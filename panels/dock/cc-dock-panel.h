/* cc-dock-panel.h
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include <shell/cc-panel.h>

G_BEGIN_DECLS

#define CC_TYPE_DOCK_PANEL (cc_dock_panel_get_type())
G_DECLARE_FINAL_TYPE (CcDockPanel, cc_dock_panel, CC, DOCK_PANEL, CcPanel)

G_END_DECLS
