#include "GCS_Copter.h"

#include "Copter.h"

const char* GCS_Copter::frame_string() const
{
    return copter.get_frame_string();
}

bool GCS_Copter::simple_input_active() const
{
    return copter.ap.simple_mode == 1;
}

bool GCS_Copter::supersimple_input_active() const
{
    return copter.ap.simple_mode == 2;
}

void GCS_Copter::update_vehicle_sensor_status_flags(void)
{
    control_sensors_present |=
        MAV_SYS_STATUS_SENSOR_ANGULAR_RATE_CONTROL |
        MAV_SYS_STATUS_SENSOR_ATTITUDE_STABILIZATION |
        MAV_SYS_STATUS_SENSOR_YAW_POSITION;

    control_sensors_enabled |=
        MAV_SYS_STATUS_SENSOR_ANGULAR_RATE_CONTROL |
        MAV_SYS_STATUS_SENSOR_ATTITUDE_STABILIZATION |
        MAV_SYS_STATUS_SENSOR_YAW_POSITION;

    control_sensors_health |=
        MAV_SYS_STATUS_SENSOR_ANGULAR_RATE_CONTROL |
        MAV_SYS_STATUS_SENSOR_ATTITUDE_STABILIZATION |
        MAV_SYS_STATUS_SENSOR_YAW_POSITION;

    const AP_GPS &gps = AP::gps();
    if (gps.status() > AP_GPS::NO_GPS) {  EXECUTE_MARK();
        control_sensors_present |= MAV_SYS_STATUS_SENSOR_GPS;
        control_sensors_enabled |= MAV_SYS_STATUS_SENSOR_GPS;
    }
#if OPTFLOW == ENABLED
    const OpticalFlow *optflow = AP::opticalflow();
    if (optflow && optflow->enabled()) {  EXECUTE_MARK();
        control_sensors_present |= MAV_SYS_STATUS_SENSOR_OPTICAL_FLOW;
        control_sensors_enabled |= MAV_SYS_STATUS_SENSOR_OPTICAL_FLOW;
    }
#endif
#if PRECISION_LANDING == ENABLED
    if (copter.precland.enabled()) {  EXECUTE_MARK();
        control_sensors_present |= MAV_SYS_STATUS_SENSOR_VISION_POSITION;
        control_sensors_enabled |= MAV_SYS_STATUS_SENSOR_VISION_POSITION;
    }
#endif
#if VISUAL_ODOMETRY_ENABLED == ENABLED
    const AP_VisualOdom *visual_odom = AP::visualodom();
    if (visual_odom && visual_odom->enabled()) {  EXECUTE_MARK();
        control_sensors_present |= MAV_SYS_STATUS_SENSOR_VISION_POSITION;
        control_sensors_enabled |= MAV_SYS_STATUS_SENSOR_VISION_POSITION;
    }
#endif
    const Copter::ap_t &ap = copter.ap;

    if (ap.rc_receiver_present) {  EXECUTE_MARK();
        control_sensors_present |= MAV_SYS_STATUS_SENSOR_RC_RECEIVER;
        control_sensors_enabled |= MAV_SYS_STATUS_SENSOR_RC_RECEIVER;
    }
#if PROXIMITY_ENABLED == ENABLED
    if (copter.g2.proximity.sensor_present()) {  EXECUTE_MARK();
        control_sensors_present |= MAV_SYS_STATUS_SENSOR_PROXIMITY;
    }
#endif
#if AC_FENCE == ENABLED
    if (copter.fence.sys_status_present()) {  EXECUTE_MARK();
        control_sensors_present |= MAV_SYS_STATUS_GEOFENCE;
    }
#endif
#if RANGEFINDER_ENABLED == ENABLED
    const RangeFinder *rangefinder = RangeFinder::get_singleton();
    if (rangefinder && rangefinder->has_orientation(ROTATION_PITCH_270)) {  EXECUTE_MARK();
        control_sensors_present |= MAV_SYS_STATUS_SENSOR_LASER_POSITION;
    }
#endif

    control_sensors_present |= MAV_SYS_STATUS_SENSOR_Z_ALTITUDE_CONTROL;
    control_sensors_present |= MAV_SYS_STATUS_SENSOR_XY_POSITION_CONTROL;

    switch (copter.control_mode) {
    case AUTO:
    case AVOID_ADSB:
    case GUIDED:
    case LOITER:
    case RTL:
    case CIRCLE:
    case LAND:
    case POSHOLD:
    case BRAKE:
    case THROW:
    case SMART_RTL:
        control_sensors_enabled |= MAV_SYS_STATUS_SENSOR_Z_ALTITUDE_CONTROL;
        control_sensors_enabled |= MAV_SYS_STATUS_SENSOR_XY_POSITION_CONTROL;
        break;
    case ALT_HOLD:
    case GUIDED_NOGPS:
    case SPORT:
    case AUTOTUNE:
    case FLOWHOLD:
        control_sensors_enabled |= MAV_SYS_STATUS_SENSOR_Z_ALTITUDE_CONTROL;
        break;
    default:
        // stabilize, acro, drift, and flip have no automatic x,y or z control (i.e. all manual)
        break;
    }

#if AC_FENCE == ENABLED
    if (copter.fence.sys_status_enabled()) {  EXECUTE_MARK();
        control_sensors_enabled |= MAV_SYS_STATUS_GEOFENCE;
    }
#endif
#if PROXIMITY_ENABLED == ENABLED
    if (copter.g2.proximity.sensor_enabled()) {  EXECUTE_MARK();
        control_sensors_enabled |= MAV_SYS_STATUS_SENSOR_PROXIMITY;
    }
#endif

    if (gps.is_healthy()) {  EXECUTE_MARK();
        control_sensors_health |= MAV_SYS_STATUS_SENSOR_GPS;
    }
    if (ap.rc_receiver_present && !copter.failsafe.radio) {  EXECUTE_MARK();
        control_sensors_health |= MAV_SYS_STATUS_SENSOR_RC_RECEIVER;
    }
#if OPTFLOW == ENABLED
    if (optflow && optflow->healthy()) {  EXECUTE_MARK();
        control_sensors_health |= MAV_SYS_STATUS_SENSOR_OPTICAL_FLOW;
    }
#endif
#if PRECISION_LANDING == ENABLED
    if (copter.precland.enabled() && copter.precland.healthy()) {  EXECUTE_MARK();
        control_sensors_health |= MAV_SYS_STATUS_SENSOR_VISION_POSITION;
    }
#endif
#if VISUAL_ODOMETRY_ENABLED == ENABLED
    if (visual_odom && visual_odom->enabled() && visual_odom->healthy()) {  EXECUTE_MARK();
        control_sensors_health |= MAV_SYS_STATUS_SENSOR_VISION_POSITION;
    }
#endif

#if PROXIMITY_ENABLED == ENABLED
    if (!copter.g2.proximity.sensor_failed()) {  EXECUTE_MARK();
        control_sensors_health |= MAV_SYS_STATUS_SENSOR_PROXIMITY;
    }
#endif

#if AP_TERRAIN_AVAILABLE && AC_TERRAIN
    switch (copter.terrain.status()) {
    case AP_Terrain::TerrainStatusDisabled:
        break;
    case AP_Terrain::TerrainStatusUnhealthy:
        // To-Do: restore unhealthy terrain status reporting once terrain is used in copter
        //control_sensors_present |= MAV_SYS_STATUS_TERRAIN;
        //control_sensors_enabled |= MAV_SYS_STATUS_TERRAIN;
        //break;
    case AP_Terrain::TerrainStatusOK:
        control_sensors_present |= MAV_SYS_STATUS_TERRAIN;
        control_sensors_enabled |= MAV_SYS_STATUS_TERRAIN;
        control_sensors_health  |= MAV_SYS_STATUS_TERRAIN;
        break;
    }
#endif

#if RANGEFINDER_ENABLED == ENABLED
    if (copter.rangefinder_state.enabled) {  EXECUTE_MARK();
        control_sensors_enabled |= MAV_SYS_STATUS_SENSOR_LASER_POSITION;
        if (rangefinder && rangefinder->has_data_orient(ROTATION_PITCH_270)) {  EXECUTE_MARK();
            control_sensors_health |= MAV_SYS_STATUS_SENSOR_LASER_POSITION;
        }
    }
#endif

#if AC_FENCE == ENABLED
    if (!copter.fence.sys_status_failed()) {  EXECUTE_MARK();
        control_sensors_health |= MAV_SYS_STATUS_GEOFENCE;
    }
#endif
}
