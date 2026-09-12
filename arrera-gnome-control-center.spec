Name:           arrera-gnome-control-center
Epoch:          1
Version:        50.4
Release:        0.1.beta1.arrera%{?dist}
Summary:        Paramètres de GNOME avec extension Arrera Dock intégrée (Version Bêta)

License:        GPL-2.0-or-later
URL:            https://gitlab.gnome.org/GNOME/gnome-control-center
Source0:        %{name}-%{version}.tar.gz

BuildRequires:  meson >= 0.60.0
BuildRequires:  ninja-build
BuildRequires:  gcc
BuildRequires:  gcc-c++
BuildRequires:  git
BuildRequires:  blueprint-compiler >= 0.19
BuildRequires:  desktop-file-utils
BuildRequires:  gettext
BuildRequires:  docbook-style-xsl
BuildRequires:  libxslt

BuildRequires:  pkgconfig(accountsservice)
BuildRequires:  pkgconfig(colord)
BuildRequires:  pkgconfig(colord-gtk4)
BuildRequires:  pkgconfig(cups)
BuildRequires:  pkgconfig(gcr-4)
BuildRequires:  pkgconfig(gdk-pixbuf-2.0)
BuildRequires:  pkgconfig(gio-2.0)
BuildRequires:  pkgconfig(gio-unix-2.0)
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(gmodule-2.0)
BuildRequires:  pkgconfig(gnome-desktop-4)
BuildRequires:  pkgconfig(gnome-settings-daemon)
BuildRequires:  pkgconfig(goa-1.0)
BuildRequires:  pkgconfig(goa-backend-1.0)
BuildRequires:  pkgconfig(gobject-2.0)
BuildRequires:  pkgconfig(gsettings-desktop-schemas)
BuildRequires:  pkgconfig(gsound)
BuildRequires:  pkgconfig(gtk4)
BuildRequires:  pkgconfig(gudev-1.0)
BuildRequires:  pkgconfig(ibus-1.0)
BuildRequires:  pkgconfig(krb5)
BuildRequires:  pkgconfig(libadwaita-1)
BuildRequires:  pkgconfig(libgtop-2.0)
BuildRequires:  pkgconfig(libnm)
BuildRequires:  pkgconfig(libnma-gtk4)
BuildRequires:  pkgconfig(libpulse)
BuildRequires:  pkgconfig(libpulse-mainloop-glib)
BuildRequires:  pkgconfig(libsecret-1)
BuildRequires:  pkgconfig(libsoup-3.0)
BuildRequires:  pkgconfig(libxml-2.0)
BuildRequires:  pkgconfig(mm-glib)
BuildRequires:  pkgconfig(polkit-gobject-1)
BuildRequires:  pkgconfig(pwquality)
BuildRequires:  pkgconfig(smbclient)
BuildRequires:  pkgconfig(tecla)
BuildRequires:  pkgconfig(udisks2)
BuildRequires:  pkgconfig(upower-glib)
BuildRequires:  pkgconfig(x11)
BuildRequires:  pkgconfig(xi)
%ifnarch s390 s390x
BuildRequires:  pkgconfig(gnome-bluetooth-3.0)
BuildRequires:  pkgconfig(libwacom)
%endif

Requires:       glib2%{?_isa}
Requires:       gtk4%{?_isa}
Requires:       libadwaita%{?_isa}
Requires:       gsettings-desktop-schemas
Requires:       accountsservice
Requires:       colord
Requires:       /usr/bin/tecla

# Remplace, rend obsolète et fournit gnome-control-center pour assurer la désinstallation automatique du paquet de base
Provides:       gnome-control-center = %{?epoch:%{epoch}:}%{version}-%{release}
Provides:       gnome-control-center%{?_isa} = %{?epoch:%{epoch}:}%{version}-%{release}
Provides:       control-center = %{?epoch:%{epoch}:}%{version}-%{release}
Provides:       control-center%{?_isa} = %{?epoch:%{epoch}:}%{version}-%{release}
Obsoletes:      gnome-control-center < %{?epoch:%{epoch}:}%{version}-%{release}
Obsoletes:      control-center < %{?epoch:%{epoch}:}%{version}-%{release}
Conflicts:      gnome-control-center

%description
Panneau de configuration GNOME Settings intégrant la configuration native
de l'extension Arrera Dock pour la distribution Arrera Linux (Version Bêta).

%prep
%autosetup -p1 -n %{name}-%{version}

%build
%meson \
  -Ddocumentation=true \
  -Dmalcontent=false \
  %{nil}
%meson_build

%install
%meson_install
%find_lang gnome-control-center-2.0 --all-name

%check
desktop-file-validate %{buildroot}%{_datadir}/applications/*.desktop

%files -f gnome-control-center-2.0.lang
%license COPYING
%doc README.md
%{_bindir}/gnome-control-center
%{_libexecdir}/gnome-control-center*
%{_datadir}/applications/*.desktop
%{_datadir}/bash-completion/completions/gnome-control-center
%{_datadir}/dbus-1/interfaces/*.xml
%{_datadir}/dbus-1/services/*.service
%{_datadir}/gettext/
%{_datadir}/glib-2.0/schemas/*.xml
%{_datadir}/gnome-control-center/
%{_datadir}/gnome-shell/search-providers/*.ini
%{_datadir}/icons/gnome-logo-text*.svg
%{_datadir}/icons/hicolor/*/*/*
%{_metainfodir}/*.xml
%{_datadir}/pixmaps/faces
%{_datadir}/pkgconfig/gnome-keybindings.pc
%{_datadir}/polkit-1/actions/*.policy
%{_datadir}/polkit-1/rules.d/*.rules
%{_datadir}/sounds/gnome/default/*/*.ogg
%{_mandir}/man1/gnome-control-center.1*

%changelog
* Sun Sep 06 2026 BaptisteP <contact@arrera-linux.org> - 1:50.4-0.1.beta1.arrera
- Version bêta initiale de arrera-gnome-control-center avec support natif du dock Arrera
