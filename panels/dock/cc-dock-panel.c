/* cc-dock-panel.c
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "cc-dock-panel.h"
#include "cc-dock-resources.h"
#include <adwaita.h>
#include <glib/gi18n.h>

struct _CcDockPanel
{
  CcPanel      parent_instance;

  GSettings   *dock_settings;

  AdwSwitchRow *autohide_row;
  AdwSwitchRow *wave_row;
  AdwSwitchRow *super_key_row;
  AdwComboRow  *position_row;
  AdwComboRow  *size_row;
  AdwComboRow  *theme_row;
};

CC_PANEL_REGISTER (CcDockPanel, cc_dock_panel)

static void
position_changed_cb (CcDockPanel *self)
{
  guint selected;
  const gchar *val;
  GSettingsSchema *schema = NULL;

  g_assert (CC_IS_DOCK_PANEL (self));

  if (!self->dock_settings)
    return;

  g_object_get (self->dock_settings, "settings-schema", &schema, NULL);
  if (schema)
    {
      gboolean has_key = g_settings_schema_has_key (schema, "position");
      g_settings_schema_unref (schema);
      if (!has_key)
        return;
    }

  selected = adw_combo_row_get_selected (self->position_row);
  if (selected == 1)
    val = "left";
  else if (selected == 2)
    val = "right";
  else
    val = "bottom";

  g_settings_set_string (self->dock_settings, "position", val);
}

static void
size_changed_cb (CcDockPanel *self)
{
  guint selected;
  const gchar *val;
  GSettingsSchema *schema = NULL;

  g_assert (CC_IS_DOCK_PANEL (self));

  if (!self->dock_settings)
    return;

  g_object_get (self->dock_settings, "settings-schema", &schema, NULL);
  if (schema)
    {
      gboolean has_key = g_settings_schema_has_key (schema, "icon-size");
      g_settings_schema_unref (schema);
      if (!has_key)
        return;
    }

  selected = adw_combo_row_get_selected (self->size_row);
  if (selected == 0)
    val = "small";
  else if (selected == 2)
    val = "large";
  else
    val = "medium";

  g_settings_set_string (self->dock_settings, "icon-size", val);
}

static void
theme_changed_cb (CcDockPanel *self)
{
  guint selected;
  const gchar *val;
  GSettingsSchema *schema = NULL;

  g_assert (CC_IS_DOCK_PANEL (self));

  if (!self->dock_settings)
    return;

  g_object_get (self->dock_settings, "settings-schema", &schema, NULL);
  if (schema)
    {
      gboolean has_key = g_settings_schema_has_key (schema, "theme-mode");
      g_settings_schema_unref (schema);
      if (!has_key)
        return;
    }

  selected = adw_combo_row_get_selected (self->theme_row);
  val = (selected == 1) ? "black-outline" : "expressive";

  g_settings_set_string (self->dock_settings, "theme-mode", val);
}

static void
cc_dock_panel_finalize (GObject *object)
{
  CcDockPanel *self = CC_DOCK_PANEL (object);

  g_clear_object (&self->dock_settings);

  G_OBJECT_CLASS (cc_dock_panel_parent_class)->finalize (object);
}

static void
cc_dock_panel_class_init (CcDockPanelClass *klass)
{
  GObjectClass   *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->finalize = cc_dock_panel_finalize;

  gtk_widget_class_set_template_from_resource (widget_class, "/org/gnome/control-center/dock/cc-dock-panel.ui");

  gtk_widget_class_bind_template_child (widget_class, CcDockPanel, autohide_row);
  gtk_widget_class_bind_template_child (widget_class, CcDockPanel, wave_row);
  gtk_widget_class_bind_template_child (widget_class, CcDockPanel, super_key_row);
  gtk_widget_class_bind_template_child (widget_class, CcDockPanel, position_row);
  gtk_widget_class_bind_template_child (widget_class, CcDockPanel, size_row);
  gtk_widget_class_bind_template_child (widget_class, CcDockPanel, theme_row);

  gtk_widget_class_bind_template_callback (widget_class, position_changed_cb);
  gtk_widget_class_bind_template_callback (widget_class, size_changed_cb);
  gtk_widget_class_bind_template_callback (widget_class, theme_changed_cb);
}

static void
cc_dock_panel_init (CcDockPanel *self)
{
  GSettingsSchemaSource *source;
  g_autoptr(GSettingsSchema) schema = NULL;

  g_resources_register (cc_dock_get_resource ());
  gtk_widget_init_template (GTK_WIDGET (self));

  source = g_settings_schema_source_get_default ();
  if (source)
    schema = g_settings_schema_source_lookup (source, "org.gnome.shell.extensions.dock", TRUE);

  if (!schema)
    {
      g_warning ("Schema 'org.gnome.shell.extensions.dock' is not installed on the system.");
      gtk_widget_set_sensitive (GTK_WIDGET (self), FALSE);
      return;
    }

  self->dock_settings = g_settings_new ("org.gnome.shell.extensions.dock");

  /* 1. Liaison des booléens */
  if (g_settings_schema_has_key (schema, "autohide"))
    {
      g_settings_bind (self->dock_settings, "autohide",
                       self->autohide_row, "active",
                       G_SETTINGS_BIND_DEFAULT);
    }
  else
    {
      gtk_widget_set_visible (GTK_WIDGET (self->autohide_row), FALSE);
    }

  if (g_settings_schema_has_key (schema, "enable-wave-effect"))
    {
      g_settings_bind (self->dock_settings, "enable-wave-effect",
                       self->wave_row, "active",
                       G_SETTINGS_BIND_DEFAULT);
    }
  else
    {
      gtk_widget_set_visible (GTK_WIDGET (self->wave_row), FALSE);
    }

  if (g_settings_schema_has_key (schema, "super-key-opens-launcher"))
    {
      g_settings_bind (self->dock_settings, "super-key-opens-launcher",
                       self->super_key_row, "active",
                       G_SETTINGS_BIND_DEFAULT);
    }
  else
    {
      gtk_widget_set_visible (GTK_WIDGET (self->super_key_row), FALSE);
    }

  /* 2. Initialisation de la position */
  if (g_settings_schema_has_key (schema, "position"))
    {
      g_autofree gchar *current_pos = g_settings_get_string (self->dock_settings, "position");
      if (g_strcmp0 (current_pos, "left") == 0)
        adw_combo_row_set_selected (self->position_row, 1);
      else if (g_strcmp0 (current_pos, "right") == 0)
        adw_combo_row_set_selected (self->position_row, 2);
      else
        adw_combo_row_set_selected (self->position_row, 0);
    }
  else
    {
      gtk_widget_set_visible (GTK_WIDGET (self->position_row), FALSE);
    }

  /* 3. Initialisation de la sélection taille */
  if (g_settings_schema_has_key (schema, "icon-size"))
    {
      g_autofree gchar *current_size = g_settings_get_string (self->dock_settings, "icon-size");
      if (g_strcmp0 (current_size, "small") == 0)
        adw_combo_row_set_selected (self->size_row, 0);
      else if (g_strcmp0 (current_size, "large") == 0)
        adw_combo_row_set_selected (self->size_row, 2);
      else
        adw_combo_row_set_selected (self->size_row, 1);
    }
  else
    {
      gtk_widget_set_visible (GTK_WIDGET (self->size_row), FALSE);
    }

  /* 4. Initialisation de la sélection thème */
  if (g_settings_schema_has_key (schema, "theme-mode"))
    {
      g_autofree gchar *current_theme = g_settings_get_string (self->dock_settings, "theme-mode");
      if (g_strcmp0 (current_theme, "black-outline") == 0)
        adw_combo_row_set_selected (self->theme_row, 1);
      else
        adw_combo_row_set_selected (self->theme_row, 0);
    }
  else
    {
      gtk_widget_set_visible (GTK_WIDGET (self->theme_row), FALSE);
    }
}
