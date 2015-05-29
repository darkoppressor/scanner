#ifndef android_h
#define android_h

#include <string>
#include <vector>

#ifdef GAME_OS_ANDROID
    #include <SDL.h>
    #include <jni.h>
#endif

const int SENSOR_TYPE_ACCELEROMETER=1;
const int SENSOR_TYPE_AMBIENT_TEMPERATURE=13;
const int SENSOR_TYPE_GAME_ROTATION_VECTOR=15;
const int SENSOR_TYPE_GEOMAGNETIC_ROTATION_VECTOR=20;
const int SENSOR_TYPE_GRAVITY=9;
const int SENSOR_TYPE_GYROSCOPE=4;
const int SENSOR_TYPE_GYROSCOPE_UNCALIBRATED=16;
const int SENSOR_TYPE_LIGHT=5;
const int SENSOR_TYPE_LINEAR_ACCELERATION=10;
const int SENSOR_TYPE_MAGNETIC_FIELD=2;
const int SENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED=14;
const int SENSOR_TYPE_PRESSURE=6;
const int SENSOR_TYPE_PROXIMITY=8;
const int SENSOR_TYPE_RELATIVE_HUMIDITY=12;
const int SENSOR_TYPE_ROTATION_VECTOR=11;
const int SENSOR_TYPE_STEP_COUNTER=19;
const int SENSOR_TYPE_STEP_DETECTOR=18;

//This value should equal the highest numbered sensor type from the above constants (which come from Android's Sensor class)
const int SENSOR_TYPE_COUNT=20;

//The highest number of values any sensor has
const int SENSOR_VALUES_MAX=6;

//The number of values we read from GPS
const int GPS_VALUES_MAX=6;

#ifdef GAME_OS_ANDROID
    extern "C"{
        extern void jni_initialize();
        extern int jni_get_sensor_number(const char* sensor_name);
        extern bool jni_get_sensor_available(const char* sensor_name);
        extern bool jni_get_sensor_enabled(const char* sensor_name);
        extern int jni_get_sensor_value_count_actual(const char* sensor_name);
        extern void jni_get_sensor_values(const char* sensor_name,float values[SENSOR_VALUES_MAX]);
        extern bool jni_get_gps_available();
        extern bool jni_get_gps_accessible();
        extern bool jni_get_gps_enabled();
        extern void jni_get_gps_values(double values[GPS_VALUES_MAX]);
    }
#endif

class Android_Sensor{
public:

    int value_count;
    std::string units;
    std::vector<std::string> value_labels;

    Android_Sensor();

    void setup(int get_value_count,std::string get_units,const std::vector<std::string>& get_value_labels);
};

class Android_GPS{
public:

    double accuracy;
    double altitude;
    double bearing;
    double latitude;
    double longitude;
    double speed;

    static const std::string UNITS_ACCURACY;
    static const std::string UNITS_ALTITUDE;
    static const std::string UNITS_BEARING;
    static const std::string UNITS_LATITUDE;
    static const std::string UNITS_LONGITUDE;
    static const std::string UNITS_SPEED;

    Android_GPS();
};

class Android{
private:

    template<typename... Args>
    void call_android_method(std::string method_name,std::string signature,Args... args){
        #ifdef GAME_OS_ANDROID
            JNIEnv* env=(JNIEnv*)SDL_AndroidGetJNIEnv();
            jobject activity=(jobject)SDL_AndroidGetActivity();
            jclass clazz(env->GetObjectClass(activity));
            jmethodID method_id=env->GetStaticMethodID(clazz,method_name.c_str(),signature.c_str());
            env->CallStaticVoidMethod(clazz,method_id,args...);
            env->DeleteLocalRef(activity);
            env->DeleteLocalRef(clazz);
        #endif
    }

    void set_sensor_enabled(std::string sensor_type,bool enabled);

    void set_gps_enabled(bool enabled,uint32_t minimum_update_time,float minimum_update_distance);

public:

    Android_Sensor sensors[SENSOR_TYPE_COUNT];

    Android();

    bool get_sensor_availability(std::string sensor_type);
    bool get_sensor_state(std::string sensor_type);
    int get_sensor_value_count_actual(std::string sensor_type);
    int get_sensor_value_count(std::string sensor_type);
    std::string get_sensor_units(std::string sensor_type);
    void get_sensor_value_labels(std::string sensor_type,std::string value_labels[SENSOR_VALUES_MAX]);
    void get_sensor_values(std::string sensor_type,float values[SENSOR_VALUES_MAX]);
    void enable_sensor(std::string sensor_type);
    void disable_sensor(std::string sensor_type);

    void vibrate(uint32_t length);
    void vibrate_stop();

    bool get_gps_availability();
    bool get_gps_accessibility();
    bool get_gps_state();
    Android_GPS get_gps_readout();
    void enable_gps(uint32_t minimum_update_time,float minimum_update_distance);
    void disable_gps();
};

#endif
