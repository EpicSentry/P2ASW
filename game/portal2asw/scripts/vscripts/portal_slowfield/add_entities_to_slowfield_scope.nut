// To be used in the scope of a logic_script entity, specifying all entities needed for the slowfield effect.
// This script just adds all specified entities to the detector's scope.

local slowfield_portal_detector = EntityGroup[0]
// TODO: Naming convention for this? Seems to be happening a lot and it would suck to confuse these two objects
slowfield_portal_detector.ValidateScriptScope()
local SCOPEOF_slowfield_portal_detector = slowfield_portal_detector.GetScriptScope()

// Todo: this could be a generic thing we do to any logic_script? The naming convention and lack of loops sucks.
SCOPEOF_slowfield_portal_detector.zoom_in <- EntityGroup[1]
SCOPEOF_slowfield_portal_detector.zoom_out <- EntityGroup[2]
SCOPEOF_slowfield_portal_detector.slowtime_color_correction <- EntityGroup[3]
SCOPEOF_slowfield_portal_detector.slowtime_sound_time_start <- EntityGroup[4]
SCOPEOF_slowfield_portal_detector.slowtime_sound_loop <- EntityGroup[5]
SCOPEOF_slowfield_portal_detector.slowtime_sound_time_end <- EntityGroup[6]