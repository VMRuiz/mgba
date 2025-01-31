/* Copyright (c) 2013-2016 Jeffrey Pfau
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
#ifndef GB_OVERRIDES_H
#define GB_OVERRIDES_H

#include <mgba-util/common.h>

CXX_GUARD_START

#include <mgba/gb/interface.h>

struct GBColorPreset {
	const char* name;
	uint32_t colors[12];
};

struct Configuration;
bool GBOverrideFind(const struct Configuration*, struct GBCartridgeOverride* override);
bool GBOverrideColorFind(struct GBCartridgeOverride* override, enum GBColorLookup);
void GBOverrideSave(struct Configuration*, const struct GBCartridgeOverride* override);

size_t GBColorPresetList(const struct GBColorPreset** presets);

struct GB;
void GBOverrideApply(struct GB*, const struct GBCartridgeOverride*);
void GBOverrideApplyDefaults(struct GB*);

CXX_GUARD_END

#endif
