#ifndef ADF_H
#define ADF_H
#include <QModbusRtuSerialMaster>
#include <QWidget>
#include <QTimer>
#include "mainwindow.h"
#include "types.h"
#include <QEventLoop>
#include "serial/DeviceSerialInterface.h"
#include "ui_mainwindow.h"

//============ структуры команд и ответов =================================================
enum{
    ADC_UD,			//CH10
    ADC_U_A,		//CH11
    ADC_U_B,		//CH12
    ADC_U_C,		//CH13
    ADC_I_A,		//CH0
    ADC_I_B,		//CH1
    ADC_I_C,		//CH2
    ADC_I_ET,		//CH3
    ADC_I_TEMP1,//CH5
    ADC_I_TEMP2,//CH6
    ADC_EMS_A,	//CH14
    ADC_EMS_B,	//CH15
    ADC_EMS_C,	//CH8
    ADC_EMS_I,
    ADC_MATH_A,
    ADC_MATH_B,
    ADC_MATH_C
};
#define ADC_CHANNEL_NUMBER ADC_MATH_A
#define ADC_MATH_NUMBER 3
enum{
    OSC_UD,
    OSC_U_A,
    OSC_U_B,
    OSC_U_C,
    OSC_I_A,
    OSC_I_B,
    OSC_I_C,
    OSC_COMP_A,
    OSC_COMP_B,
    OSC_COMP_C,
    OSC_CHANNEL_NUMBER
};
#pragma pack(1)
struct sCOMMAND_GET_ADC_ACTIVE{
    uint8_t null;
};
struct sANSWER_GET_ADC_ACTIVE{
    float ADC_UD;			//CH10
    float ADC_U_A;		//CH11
    float ADC_U_B;		//CH12
    float ADC_U_C;		//CH13
    float ADC_I_A;		//CH0
    float ADC_I_B;		//CH1
    float ADC_I_C;		//CH2
    float ADC_I_ET;		//CH3
    float ADC_I_TEMP1;//CH5
    float ADC_I_TEMP2;//CH6
    float ADC_EMS_A;  //CH14
    float ADC_EMS_B;	//CH15
    float ADC_EMS_C;  //CH8
    float ADC_EMS_I;	//CH9
    float ADC_MATH_A;	//CH15
    float ADC_MATH_B;  //CH8
    float ADC_MATH_C;	//CH9
};
struct sCOMMAND_GET_ADC_ACTIVE_RAW{
        uint8_t null;
};
struct sANSWER_GET_ADC_ACTIVE_RAW{
    uint16_t ADC_UD;			//CH10
    uint16_t ADC_U_A;		//CH11
    uint16_t ADC_U_B;		//CH12
    uint16_t ADC_U_C;		//CH13
    uint16_t ADC_I_A;		//CH0
    uint16_t ADC_I_B;		//CH1
    uint16_t ADC_I_C;		//CH2
    uint16_t ADC_I_ET;		//CH3
    uint16_t ADC_I_TEMP1;//CH5
    uint16_t ADC_I_TEMP2;//CH6
    uint16_t ADC_EMS_A;  //CH14
    uint16_t ADC_EMS_B;	//CH15
    uint16_t ADC_EMS_C;  //CH8
    uint16_t ADC_EMS_I;	//CH9
};
struct sCOMMAND_SWITCH_ON_OFF{
    uint8_t  command;
    uint32_t data;
};
struct sANSWER_SWITCH_ON_OFF{
  uint8_t result;
};
struct sCOMMAND_GET_WORK_STATE{
  uint64_t currentTime;
};
struct sANSWER_GET_WORK_STATE{
    uint8_t  state;
    uint32_t activeChannels[3];
};

struct sCOMMAND_GET_VERSION_INFO{
        uint8_t null;
};
struct sANSWER_GET_VERSION_INFO{
    uint16_t major;
    uint16_t minor;
    uint16_t micro;
    uint16_t build;

    uint8_t day;
    uint8_t month;
    uint16_t year;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
};
struct sCOMMAND_GET_OSCILOG{
    uint8_t num;
};
#define OSCILLOG_TRANSFER_SIZE 128
struct sANSWER_GET_OSCILOG{
    uint8_t ch;
    float max;
    float min;
    uint16_t len;
    uint8_t data[OSCILLOG_TRANSFER_SIZE];
};
struct sCOMMAND_GET_SETTINGS_CALIBRATIONS{
        uint8_t null;
};
struct sANSWER_GET_SETTINGS_CALIBRATIONS{
    float calibration[ADC_CHANNEL_NUMBER];
    float offset[ADC_CHANNEL_NUMBER];
};
struct sCOMMAND_GET_SETTINGS_PROTECTION{
        uint8_t null;
};
struct sANSWER_GET_SETTINGS_PROTECTION{
    float Ud_min;
    float Ud_max;
    float temperature;
    float U_min;
    float U_max;
    float Fnet_min;
    float Fnet_max;
    float I_max_rms;
    float I_max_peak;
};
struct sCOMMAND_GET_SETTINGS_CAPACITORS{
        uint8_t null;
};
struct sANSWER_GET_SETTINGS_CAPACITORS{
    float ctrlUd_Kp;
    float ctrlUd_Ki;
    float ctrlUd_Kd;
    float Ud_nominal;
    float Ud_precharge;
};
struct sCOMMAND_GET_SETTINGS_FILTERS{
        uint8_t null;
};
struct sANSWER_GET_SETTINGS_FILTERS{
    float K_I;
    float K_U;
    float K_UD;
};
struct sCOMMAND_GET_NET_PARAMS{
        uint8_t null;
};
struct sANSWER_GET_NET_PARAMS{
    float period_fact;
    float U0Hz_A;
    float U0Hz_B;
    float U0Hz_C;
    float I0Hz_A;
    float I0Hz_B;
    float I0Hz_C;
    float thdu_A;
    float thdu_B;
    float thdu_C;
    float U_phase_A;
    float U_phase_B;
    float U_phase_C;
};
struct sEventRecord{
    uint64_t unixTime_s_ms;

    uint32_t type;
    uint32_t info;
    float    value;
};
#define MAX_NUM_TRANSFERED_EVENTS (120/(sizeof(struct sEventRecord)))//not more than 120 bytes
struct sCOMMAND_GET_EVENTS{
    uint64_t afterIndex;
};
struct sANSWER_GET_EVENTS{
    uint16_t num;
    struct sEventRecord events[MAX_NUM_TRANSFERED_EVENTS];
};
enum{
    EVENT_TYPE_POWER,
    EVENT_TYPE_CHANGESTATE,
    EVENT_TYPE_PROTECTION,
    EVENT_TYPE_EVENT
};
enum{
    SUB_EVENT_TYPE_POWER_ON
};
enum{
    SUB_EVENT_TYPE_PROTECTION_UD_MIN,
    SUB_EVENT_TYPE_PROTECTION_UD_MAX,
    SUB_EVENT_TYPE_PROTECTION_TEMPERATURE,
    SUB_EVENT_TYPE_PROTECTION_U_MIN,
    SUB_EVENT_TYPE_PROTECTION_U_MAX,
    SUB_EVENT_TYPE_PROTECTION_F_MIN,
    SUB_EVENT_TYPE_PROTECTION_F_MAX,
    SUB_EVENT_TYPE_PROTECTION_IAFG_MAX_RMS,
    SUB_EVENT_TYPE_PROTECTION_IAFG_MAX_PEAK,
    SUB_EVENT_TYPE_PROTECTION_PHASES,
    SUB_EVENT_TYPE_PROTECTION_ADC_OVERLOAD,
    SUB_EVENT_TYPE_PROTECTION_BAD_SYNC,
    SUB_EVENT_TYPE_PROTECTION_IGBT,
};

enum{
    PROTECTION_IGNORE,
    PROTECTION_WARNING_STOP,
    PROTECTION_ERROR_STOP
};

#define sCOMMAND_SET_SETTINGS_CALIBRATIONS 	sANSWER_GET_SETTINGS_CALIBRATIONS
#define sANSWER_SET_SETTINGS_CALIBRATIONS  	sCOMMAND_GET_SETTINGS_CALIBRATIONS
#define sCOMMAND_SET_SETTINGS_PROTECTION    sANSWER_GET_SETTINGS_PROTECTION
#define sANSWER_SET_SETTINGS_PROTECTION     sCOMMAND_GET_SETTINGS_PROTECTION
#define sCOMMAND_SET_SETTINGS_CAPACITORS 	sANSWER_GET_SETTINGS_CAPACITORS
#define sANSWER_SET_SETTINGS_CAPACITORS  	sCOMMAND_GET_SETTINGS_CAPACITORS
#define sCOMMAND_SET_SETTINGS_FILTERS 		sANSWER_GET_SETTINGS_FILTERS
#define sANSWER_SET_SETTINGS_FILTERS  		sCOMMAND_GET_SETTINGS_FILTERS
//=============== Номера команд и ответов =================================================
enum P_AFG_COMMANDS {
    P_AFG_COMMAND_TEST, //тест соединения
    P_AFG_COMMAND_SWITCH_ON_OFF, //включение\выключение фильтра
    P_AFG_COMMAND_GET_ADC_ACTIVE, //получение действуюих значений
    P_AFG_COMMAND_GET_ADC_ACTIVE_RAW, // получение сырых действующих значений (без калибровки)
    P_AFG_COMMAND_GET_OSCILOG, //получение осцилограммы
    P_AFG_COMMAND_GET_NET_PARAMS,

    P_AFG_COMMAND_SET_SETTINGS_CALIBRATIONS, //Установить настройки
    P_AFG_COMMAND_SET_SETTINGS_PROTECTION, //Установить настройки
    P_AFG_COMMAND_SET_SETTINGS_CAPACITORS, //Установить настройки
    P_AFG_COMMAND_SET_SETTINGS_FILTERS,

    P_AFG_COMMAND_GET_SETTINGS_CALIBRATIONS, //Установить настройки
    P_AFG_COMMAND_GET_SETTINGS_PROTECTION, //Установить настройки
    P_AFG_COMMAND_GET_SETTINGS_CAPACITORS, //Установить настройки
    P_AFG_COMMAND_GET_SETTINGS_FILTERS,

    P_AFG_COMMAND_GET_WORK_STATE,

    P_AFG_COMMAND_GET_VERSION_INFO, // Получить информацию о версии прошивки
    P_AFG_COMMAND_GET_EVENTS,

    lenP_AFG_COMMANDS
};
//============================================================================
/*typedef enum {
    STOP, //не работает
    SYNC, //синхронизация с сетью
    PRECHARGE, //предзаряд
    CHARGE, //заряд (накачка)
    TEST, //тестирование сети
    RUN, //работает, но без компенсации
    WORK, //работает с компенсацией
    FAULT //ошибка
} AFG_STATUS;*/
//============================================================================
class PFC : public QObject{
    Q_OBJECT

public:
    ADFSerialInterface *_interface;
    //typedef void ( *prot_handler )          ( void * );
    //prot_handler handlers[lenP_AFG_COMMANDS];
    QString handlers[lenP_AFG_COMMANDS];
    PFC(){
        QThread* thread = new QThread;
        // Передаем права владения "рабочим" классом, классу QThread.
        _interface = new ADFSerialInterface(NULL);
        _interface->moveToThread(thread);
        // По завершению выходим из потока, и удаляем рабочий класс
     //   QObject::connect(_interface, SIGNAL(finished()), thread, SLOT(quit()));
     //   QObject::connect(_interface, SIGNAL(finished()), _interface, SLOT(deleteLater()));
        // Удаляем поток, после выполнения операции
        connect(thread,  SIGNAL(finished()), thread, SLOT(deleteLater()));
        connect(thread,  SIGNAL(started()), _interface, SLOT(run()));
        connect(_interface,SIGNAL(ConnectionChanged(bool)),this,SLOT(ConnectionChanged(bool)));
        thread->start();

        for (int i = 0; i < lenP_AFG_COMMANDS; i++)
                handlers[i] = "protocol_unknow_command_handle";
//==================================================================
//==================== Массив обработчиков =========================
//==================================================================
        handlers[P_AFG_COMMAND_GET_ADC_ACTIVE] =
                "protocol_GET_ADC_ACTIVE";
        handlers[P_AFG_COMMAND_SWITCH_ON_OFF] =
                "protocol_SWITCH_ON_OFF";
        handlers[P_AFG_COMMAND_GET_ADC_ACTIVE_RAW] =
                "protocol_GET_ADC_ACTIVE_RAW";
        handlers[P_AFG_COMMAND_GET_OSCILOG] =
                "protocol_GET_OSCILOG";
        handlers[P_AFG_COMMAND_GET_NET_PARAMS] =
                "protocol_GET_NET_PARAMS";
        handlers[P_AFG_COMMAND_GET_WORK_STATE] =
                "protocol_GET_WORK_STATE";
        handlers[P_AFG_COMMAND_GET_VERSION_INFO] =
                "protocol_GET_VERSION_INFO";
        handlers[P_AFG_COMMAND_GET_EVENTS] =
                "protocol_GET_EVENTS";

        handlers[P_AFG_COMMAND_GET_SETTINGS_CALIBRATIONS] =
                "protocol_GET_SETTINGS_CALIBRATIONS";
        handlers[P_AFG_COMMAND_GET_SETTINGS_PROTECTION] =
                "protocol_GET_SETTINGS_PROTECTION";
        handlers[P_AFG_COMMAND_GET_SETTINGS_CAPACITORS] =
                "protocol_GET_SETTINGS_CAPACITORS";       
        handlers[P_AFG_COMMAND_GET_SETTINGS_FILTERS] =
                "protocol_GET_SETTINGS_FILTERS";

        handlers[P_AFG_COMMAND_SET_SETTINGS_CALIBRATIONS] =
                "protocol_SET_SETTINGS_CALIBRATIONS";
        handlers[P_AFG_COMMAND_SET_SETTINGS_PROTECTION] =
                "protocol_SET_SETTINGS_PROTECTION";
        handlers[P_AFG_COMMAND_SET_SETTINGS_CAPACITORS] =
                "protocol_SET_SETTINGS_CAPACITORS";
        handlers[P_AFG_COMMAND_SET_SETTINGS_FILTERS] =
                "protocol_SET_SETTINGS_FILTERS";
//==================================================================
//==================================================================
//==================================================================
        connect(_interface,SIGNAL(Message(quint8,quint8,quint8,QString)),
                this, SIGNAL(Message(quint8,quint8,quint8,QString)));
        Message(MESSAGE_TYPE_GENERAL,MESSAGE_NORMAL,MESSAGE_TARGET_DEBUG,"Поток АДФ запущен");
    }
    void protocol_unknow_command_handle(std::vector<uint8_t> data){
        Q_UNUSED(data);
        Message(MESSAGE_TYPE_GENERAL,MESSAGE_NORMAL,MESSAGE_TARGET_DEBUG,
                QString("Неизвестная команда"));
    }
    #define HANDLER(COMM) \
        void protocol_##COMM(std::vector<uint8_t> data){\
        Q_UNUSED(data) \
        struct sANSWER_##COMM* pANSWER=(struct sANSWER_##COMM*)(&(data[0])); \
        Q_UNUSED(pANSWER)
public slots:
//==================================================================
//================  Ответы  ========================================
//==================================================================
    HANDLER(SWITCH_ON_OFF)
         emit setSwitchOnOff(pANSWER->result);
    }
//==================================================================
    HANDLER(GET_ADC_ACTIVE)
         emit setNetVoltage(pANSWER->ADC_UD,
                            pANSWER->ADC_U_A,
                            pANSWER->ADC_U_B,
                            pANSWER->ADC_U_C,
                            pANSWER->ADC_I_A,
                            pANSWER->ADC_I_B,
                            pANSWER->ADC_I_C,
                            pANSWER->ADC_I_ET,
                            pANSWER->ADC_I_TEMP1,
                            pANSWER->ADC_I_TEMP2,
                            pANSWER->ADC_EMS_A,
                            pANSWER->ADC_EMS_B,
                            pANSWER->ADC_EMS_C,
                            pANSWER->ADC_EMS_I,
                            pANSWER->ADC_MATH_A,
                            pANSWER->ADC_MATH_B,
                            pANSWER->ADC_MATH_C
                            );
    }
//==================================================================
    HANDLER(GET_ADC_ACTIVE_RAW)
         emit setNetVoltageRAW(pANSWER->ADC_UD,
                               pANSWER->ADC_U_A,
                               pANSWER->ADC_U_B,
                               pANSWER->ADC_U_C,
                               pANSWER->ADC_I_A,
                               pANSWER->ADC_I_B,
                               pANSWER->ADC_I_C,
                               pANSWER->ADC_I_ET,
                               pANSWER->ADC_I_TEMP1,
                               pANSWER->ADC_I_TEMP2,
                               pANSWER->ADC_EMS_A,
                               pANSWER->ADC_EMS_B,
                               pANSWER->ADC_EMS_C,
                               pANSWER->ADC_EMS_I
                               );
    }
//==================================================================
    HANDLER(GET_WORK_STATE)
         emit setWorkState(pANSWER->state,
                           pANSWER->activeChannels[0],
                           pANSWER->activeChannels[1],
                           pANSWER->activeChannels[2]);
    }
//==================================================================
    HANDLER(GET_EVENTS)
         if(pANSWER->num==0)return;
         QVector<struct sEventRecord> ev;
         for(int i=0;i<pANSWER->num;i++){
             ev.append(pANSWER->events[i]);
         }
         emit setEvents(ev);
    }
//==================================================================
    HANDLER(GET_VERSION_INFO)
         emit setVersionInfo(
            pANSWER->major,
            pANSWER->minor,
            pANSWER->micro,
            pANSWER->build,
            pANSWER->day,
            pANSWER->month,
            pANSWER->year,
            pANSWER->hour,
            pANSWER->minute,
            pANSWER->second);
    }
//==================================================================
    inline float line_float(float x, float x1, float y1, float x2, float y2) {
        float a;
        a = (y2 - y1) / (x2 - x1);
        return y1 + a * (x - x1);
    }

    #define shifted(x, offset) ((float)(x) + (float)(offset))
    #define unshifted(x, offset) ((float)(x) - (float)(offset))

    void interpolate_shift_resize_float(QVector<double> &in, unsigned int in_size,
            float offset, QVector<double> &out) {
        float x,x1, x2, y1, y2;
        out.resize(in_size);
        int32_t i,j,j_1;
        j=offset;
        j_1=j+1;
        float diff=offset-j;
        for (i = 0; i < in_size; i++,j++) {
            while(j>=in_size)j-=in_size;
            while(j_1>=in_size)j_1-=in_size;

            x1=j;
            y1=in[j];

            x2=j_1;
            y2=in[j_1];

            x=diff+x1;

            out[i] = line_float(x, x1, y1, x2, y2);
        }
    }
    HANDLER(GET_OSCILOG)
         float astep=(pANSWER->max-pANSWER->min)/255.0;
         uint16_t len    = pANSWER->len&0xFFFF;
         uint16_t phase  = (pANSWER->len>>16)&0xFFFF;
         QVector<double> yval(128);
         Q_UNUSED(phase);
         //qDebug()<<"Osc get";
         if(len>128){
             qDebug()<<"Wrong len";
             return;
         }

         for(int i=0;i<len;i++){
             yval[i]=pANSWER->min+astep*pANSWER->data[i];
         }

         setOscillog(pANSWER->ch,yval.toStdVector());
    }
//==================================================================
     HANDLER(GET_NET_PARAMS)
          emit setNetParams(pANSWER->period_fact,
                            pANSWER->U0Hz_A,//Постоянная составляющая
                            pANSWER->U0Hz_B,
                            pANSWER->U0Hz_C,
                            pANSWER->I0Hz_A,//Постоянная составляющая
                            pANSWER->I0Hz_B,
                            pANSWER->I0Hz_C,
                            pANSWER->thdu_A,
                            pANSWER->thdu_B,
                            pANSWER->thdu_C,
                            pANSWER->U_phase_A,
                            pANSWER->U_phase_B,
                            pANSWER->U_phase_C);
     }
//==================================================================
    HANDLER(GET_SETTINGS_CALIBRATIONS)
        QVector<float> calibration(ADC_CHANNEL_NUMBER); //!< Калибровки для каналов
        QVector<float> offset(ADC_CHANNEL_NUMBER); //!< Смещения для каналов
        for(int i=0;i<calibration.size();i++)   calibration[i]=pANSWER->calibration[i];
        for(int i=0;i<offset.size();i++)        offset[i]=pANSWER->offset[i];
        setSettingsCalibrations(
                calibration,
                offset
                );
    }
//==================================================================
    HANDLER(GET_SETTINGS_PROTECTION)
        setSettingsProtection(
            pANSWER->Ud_min,
            pANSWER->Ud_max,
            pANSWER->temperature,
            pANSWER->U_min,
            pANSWER->U_max,
            pANSWER->Fnet_min,
            pANSWER->Fnet_max,
            pANSWER->I_max_rms,
            pANSWER->I_max_peak);
    }
//==================================================================
    HANDLER(GET_SETTINGS_CAPACITORS)
        setSettingsCapacitors(
            pANSWER->ctrlUd_Kp,
            pANSWER->ctrlUd_Ki,
            pANSWER->ctrlUd_Kd,
            pANSWER->Ud_nominal,
            pANSWER->Ud_precharge);
    }
//==================================================================
    HANDLER(GET_SETTINGS_FILTERS)
        setSettingsFilters(
            pANSWER->K_I,
            pANSWER->K_U,
            pANSWER->K_UD);
    }
//==================================================================
    HANDLER(SET_SETTINGS_CALIBRATIONS)
        ansSettingsCalibrations(true);
    }
//==================================================================
    HANDLER(SET_SETTINGS_PROTECTION)
        ansSettingsProtection(true);
    }
//==================================================================
    HANDLER(SET_SETTINGS_FILTERS)
        ansSettingsFilters(true);
    }
//==================================================================
    HANDLER(SET_SETTINGS_CAPACITORS)
        ansSettingsCapacitors(true);
    }
//==================================================================
//==================================================================
//==================================================================
signals:
    void Message(quint8 type, quint8 level, quint8 target, QString message);
//==================================================================
//================ Сигналы в интерфейс =============================
//==================================================================
    void setOscillog(uint16_t channel, std::vector<double> data);
    void setNetVoltage(	float ADC_UD,
                        float ADC_U_A,
                        float ADC_U_B,
                        float ADC_U_C,
                        float ADC_I_A,
                        float ADC_I_B,
                        float ADC_I_C,
                        float ADC_I_ET,
                        float ADC_I_TEMP1,
                        float ADC_I_TEMP2,
                        float ADC_EMS_A,
                        float ADC_EMS_B,
                        float ADC_EMS_C,
                        float ADC_EMS_I,
                        float ADC_MATH_A,
                        float ADC_MATH_B,
                        float ADC_MATH_C);
    void setNetVoltageRAW(float ADC_UD,
                          float ADC_U_A,
                          float ADC_U_B,
                          float ADC_U_C,
                          float ADC_I_A,
                          float ADC_I_B,
                          float ADC_I_C,
                          float ADC_I_ET,
                          float ADC_I_TEMP1,
                          float ADC_I_TEMP2,
                          float ADC_EMS_A,
                          float ADC_EMS_B,
                          float ADC_EMS_C,
                          float ADC_EMS_I);
    void setWorkState(uint32_t state, uint32_t chA, uint32_t chB, uint32_t chC);
    void setEvents(QVector<struct sEventRecord> ev);
    void setSwitchOnOff(uint32_t result);
    void setVersionInfo(
            uint32_t major,
            uint32_t minor,
            uint32_t micro,
            uint32_t build,
            uint32_t day,
            uint32_t month,
            uint32_t year,
            uint32_t hour,
            uint32_t minute,
            uint32_t second);
    void setConnection(bool connected);
    void setNetParams(float period_fact,
                    float U0Hz_A,//Постоянная составляющая
                    float U0Hz_B,
                    float U0Hz_C,
                    float I0Hz_A,//Постоянная составляющая
                    float I0Hz_B,
                    float I0Hz_C,
                    float thdu_A,
                    float thdu_B,
                    float thdu_C,
                    float U_phase_A,
                    float U_phase_B,
                    float U_phase_C);
    void setSettingsCalibrations(
            //=== калибровки АЦП
            QVector<float> calibration, //!< Калибровки для каналов
            QVector<float> offset //!< Смещения для каналов
            );
    void setSettingsProtection(
            //=== защиты ======
            float Ud_min, //!< Граничные значения для Ud
            float Ud_max, //!< Граничные значения для Ud
            float temperature, //!< Граничные значения для Температуры
            float U_min, //!< Граничные значения для напряжения
            float U_max,
            float Fnet_min, //!< минимальная частота сети
            float Fnet_max, //!< максимальная частота сети
            float I_max_rms, //!< Максимальное граничное значение тока фильтра по RMS
            float I_max_peak //!< Максимальное граничное мгновенное значение тока фильтра
            );
    void setSettingsCapacitors(
            float ctrlUd_Kp, //!< Управление накачкой
            float ctrlUd_Ki,
            float ctrlUd_Kd,
            float Ud_nominal,
            float Ud_precharge);
    void setSettingsFilters(
            float K_I,
            float K_U,
            float K_UD);
    void setSettingsHarmonicsSuppression(
            QVector<float> fft_suppress);   
    void ansSettingsCalibrations(bool writed);
    void ansSettingsProtection(bool writed);
    void ansSettingsFilters(bool writed);
    void ansSettingsCapacitors(bool writed);
//==================================================================
//==================================================================
//==================================================================
public slots:
    void ConnectionChanged(bool connected){
        setConnection(connected);
    }
#define START_REQUEST(COMM)\
    struct sCOMMAND_##COMM req;

#define END_REQUEST(COMM) \
    std::vector<uint8_t> datareq;\
    if(sizeof(req)){\
        datareq.assign((uint8_t*)&req,(uint8_t*)&req+sizeof(req));\
    }\
    PackageCommand *p = new PackageCommand();\
    p->package_out->fill(\
                ADFMessagePriority::NORMAL,\
                Sender::GUI,\
                P_AFG_COMMAND_##COMM,\
                datareq\
             );\
    _interface->enqueueCommand(p);\
    connect(p, &PackageCommand::complete, this, &getAnswer/*,Qt::DirectConnection*/);
#define END_REQUESTP(COMM,PRIOR) \
    std::vector<uint8_t> datareq;\
    if(sizeof(req)){\
        datareq.assign((uint8_t*)&req,(uint8_t*)&req+sizeof(req));\
    }\
    PackageCommand *p = new PackageCommand();\
    p->package_out->fill(\
                PRIOR,\
                Sender::GUI,\
                P_AFG_COMMAND_##COMM,\
                datareq\
             );\
    _interface->enqueueCommand(p);\
    connect(p, &PackageCommand::complete, this, &getAnswer/*,Qt::DirectConnection*/);
//==================================================================
//===============  Запросы  ========================================
//==================================================================
    void updateNetVoltage(){
        START_REQUEST(GET_ADC_ACTIVE)
        END_REQUEST(GET_ADC_ACTIVE);
    }
    void updateWorkState(uint64_t currentTime){
        START_REQUEST(GET_WORK_STATE);
        req.currentTime=currentTime;
        END_REQUEST(GET_WORK_STATE);
    }
    void updateEvents(uint64_t afterIndex){
        START_REQUEST(GET_EVENTS);
        req.afterIndex=afterIndex;
        END_REQUEST(GET_EVENTS);
    }
    void updateVersionInfo(){
        START_REQUEST(GET_VERSION_INFO)
        END_REQUEST(GET_VERSION_INFO);
    }
    void updateNetVoltageRAW(){
        START_REQUEST(GET_ADC_ACTIVE_RAW)
        END_REQUEST(GET_ADC_ACTIVE_RAW);
    }
    void updateOscillog(uint16_t channel){
        START_REQUEST(GET_OSCILOG);
        req.num=channel;
        END_REQUEST(GET_OSCILOG);
    }
    void updateNetParams(){
        START_REQUEST(GET_NET_PARAMS)
        END_REQUEST(GET_NET_PARAMS);
    }
    void updateSettingsCalibrations(){
        START_REQUEST(GET_SETTINGS_CALIBRATIONS);
        END_REQUEST(GET_SETTINGS_CALIBRATIONS);
    }
    void updateSettingsProtection(){
        START_REQUEST(GET_SETTINGS_PROTECTION);
        END_REQUEST(GET_SETTINGS_PROTECTION);
    }
    void updateSettingsCapacitors(){
        START_REQUEST(GET_SETTINGS_CAPACITORS);
        END_REQUEST(GET_SETTINGS_CAPACITORS);
    }
    void updateSettingsFilters(){
        START_REQUEST(GET_SETTINGS_FILTERS);
        END_REQUEST(GET_SETTINGS_FILTERS);
    }    
//==================================================================
//====================  Запись параметров ==========================
//==================================================================
    void writeSettingsCalibrations(
            //=== калибровки АЦП
            QVector<float> calibration, //!< Калибровки для каналов
            QVector<float> offset //!< Смещения для каналов
            ){
        START_REQUEST(SET_SETTINGS_CALIBRATIONS);
        for(int i=0;i<calibration.size();i++)req.calibration[i]=calibration[i];
        for(int i=0;i<offset.size();i++)req.offset[i]=offset[i];
        END_REQUEST(SET_SETTINGS_CALIBRATIONS);
    }
//==================================================================
    void writeSettingsProtection(
            //=== защиты ======
            float Ud_min, //!< Граничные значения для Ud
            float Ud_max, //!< Граничные значения для Ud
            float temperature, //!< Граничные значения для Температуры
            float U_min, //!< Граничные значения для напряжения
            float U_max,
            float Fnet_min, //!< минимальная частота сети
            float Fnet_max, //!< максимальная частота сети
            float I_max_rms, //!< Максимальное граничное значение тока фильтра по RMS
            float I_max_peak //!< Максимальное граничное мгновенное значение тока фильтра
            ){
        START_REQUEST(SET_SETTINGS_PROTECTION);
            req.Ud_min=Ud_min;
            req.Ud_max=Ud_max;
            req.temperature=temperature;
            req.U_min=U_min;
            req.U_max=U_max;
            req.Fnet_min=Fnet_min;
            req.Fnet_max=Fnet_max;
            req.I_max_rms=I_max_rms;
            req.I_max_peak=I_max_peak;
        END_REQUEST(SET_SETTINGS_PROTECTION);
    }
//==================================================================
    void writeSettingsCapacitors(
            float ctrlUd_Kp, //!< Управление накачкой
            float ctrlUd_Ki,
            float ctrlUd_Kd,
            float Ud_nominal,
            float Ud_precharge){
        START_REQUEST(SET_SETTINGS_CAPACITORS);
            req.ctrlUd_Kp=ctrlUd_Kp;
            req.ctrlUd_Ki=ctrlUd_Ki;
            req.ctrlUd_Kd=ctrlUd_Kd;
            req.Ud_nominal=Ud_nominal;
            req.Ud_precharge=Ud_precharge;
        END_REQUEST(SET_SETTINGS_CAPACITORS);
    }
//==================================================================
    void writeSettingsFilters(
            float K_I,
            float K_U,
            float K_UD){
        START_REQUEST(SET_SETTINGS_FILTERS);
            req.K_I         =K_I;
            req.K_U         =K_U;
            req.K_UD        =K_UD;
        END_REQUEST(SET_SETTINGS_FILTERS);
    }

//==================================================================
    void writeSwitchOnOff(uint32_t command,uint32_t data){
        START_REQUEST(SWITCH_ON_OFF)
             req.command=command;
             req.data=data;
        END_REQUESTP(SWITCH_ON_OFF,ADFMessagePriority::HIGH);
    }
//==================================================================
//==================================================================
//==================================================================
    void getAnswer(bool is_timeout, PackageCommand *pc){
        //qDebug() << Q_FUNC_INFO;
        if(is_timeout)return;
        if(pc->package_in->command()>=lenP_AFG_COMMANDS){
            Message(MESSAGE_TYPE_CONNECTION,MESSAGE_NORMAL,MESSAGE_TARGET_DEBUG,"Неизвестный ответ");
            return;
        }
        //qDebug() << pc->package_in->command();
        if(pc->package_in->error()){
            Message(MESSAGE_TYPE_GLOBALWARNING,MESSAGE_NORMAL,MESSAGE_TARGET_DEBUG,"Ошибка выполнения команды");
            return;
        }
        std::vector<uint8_t> vec(pc->package_in->data());

        QMetaObject::invokeMethod(this,handlers[pc->package_in->command()].toStdString().data(),Qt::QueuedConnection,
                    Q_ARG(std::vector<uint8_t>,vec));
        //QTimer::singleShot(100, [this,apc]() { handlers[pc->package_in->command()](&apc); } );
        //handlers[pc->package_in->command()](pc);
    }
};

#endif // ADF_H
