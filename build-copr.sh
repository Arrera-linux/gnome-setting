#!/usr/bin/env bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

PKG_NAME="arrera-gnome-control-center"
VERSION="50.4"
SPEC_FILE="${PKG_NAME}.spec"
DEFAULT_REPO="arrera-software/arrera_blue"

echo "=================================================="
echo "📦 Préparation du paquet RPM pour Copr"
echo "=================================================="

# 1. Vérification des outils requis
echo "🔍 [1/4] Vérification des outils nécessaires..."
for tool in rpmbuild copr-cli tar; do
    if ! command -v "$tool" >/dev/null 2>&1; then
        echo "❌ Outil manquant : $tool. Veuillez l'installer (ex: sudo dnf install $tool)."
        exit 1
    fi
done

# 2. Préparation de l'arborescence rpmbuild
echo "📁 [2/4] Préparation de l'environnement rpmbuild..."
RPMBUILD_DIR="$HOME/rpmbuild"
mkdir -p "$RPMBUILD_DIR"/{BUILD,BUILDROOT,RPMS,SOURCES,SPECS,SRPMS}

# 3. Création de l'archive source tarball (inclut les sous-projets mais exclut les dossiers temporaires)
echo "🗜️ [3/4] Création de l'archive source (${PKG_NAME}-${VERSION}.tar.gz)..."
TMP_STAGE="$(mktemp -d)"
mkdir -p "$TMP_STAGE/${PKG_NAME}-${VERSION}"

# Copier tous les fichiers sources dans le dossier temporaire
tar --exclude='./.git' \
    --exclude='./_build' \
    --exclude='./build' \
    --exclude='./subprojects/gvc/.git' \
    --exclude='./subprojects/libgxdp/.git' \
    -cf - . | (cd "$TMP_STAGE/${PKG_NAME}-${VERSION}" && tar -xf -)

# Générer l'archive .tar.gz finale dans rpmbuild/SOURCES
tar -czf "$RPMBUILD_DIR/SOURCES/${PKG_NAME}-${VERSION}.tar.gz" -C "$TMP_STAGE" "${PKG_NAME}-${VERSION}"
rm -rf "$TMP_STAGE"

# 4. Génération du SRPM (.src.rpm)
echo "📦 [4/4] Création du fichier source RPM (SRPM)..."
cp "$SPEC_FILE" "$RPMBUILD_DIR/SPECS/"
rpmbuild -bs "$RPMBUILD_DIR/SPECS/$SPEC_FILE"

SRPM_FILE=$(ls -t "$RPMBUILD_DIR/SRPMS/${PKG_NAME}-${VERSION}"*.src.rpm | head -n 1)
echo "✅ SRPM généré avec succès : $SRPM_FILE"

# 5. Envoi vers Fedora Copr
COPR_REPO="${1:-$DEFAULT_REPO}"

echo ""
echo "=================================================="
echo "🚀 Envoi vers Fedora Copr : $COPR_REPO"
echo "=================================================="
echo "   (Copr va compiler automatiquement pour x86_64 et aarch64)"
copr-cli build "$COPR_REPO" "$SRPM_FILE"

echo ""
echo "🎉 Le build a été soumis avec succès à Copr !"
echo "Suivez la progression sur : https://copr.fedorainfracloud.org/coprs/$COPR_REPO/builds/"
