#define CMD_RESET                 0x01
#define CMD_TURN_ON_LED           0x02
#define CMD_TURN_OFF_LED          0x03
#define CMD_TURN_ON_3V3           0x04
#define CMD_TURN_OFF_3V3          0x05
#define CMD_TURN_ON_5V            0x06
#define CMD_TURN_OFF_5V           0x07
#define CMD_TURN_ON_3V3_5V        0x08
#define CMD_TURN_OFF_3V3_5V       0x09

#define CMD_READ_SIPM_TEMP        0x0A
#define CMD_READ_UC_TEMP          0X0B

#define CMD_READ_EPRM_BYTE        0X0C
#define CMD_READ_EPRM_WORD        0X0D
#define CMD_WRITE_EPRM_BYTE       0X0E
#define CMD_WRITE_EPRM_WORD       0X0F

#define CMD_INIT_MUSIC                      0x10
#define CMD_LOAD_MUSIC_CONFIG_EPRM          0x11
#define CMD_PRINT_LOADED_MUSIC_CONFIG       0x12
#define CMD_WRITE_LOADED_CONFIG_TO_MUSIC    0x13
#define CMD_READ_FROM_MUSIC_REGISTER        0x14
#define CMD_WRITE_TO_MUSIC_REGISTER         0x15

#define CMD_SET_ADC_RATE          0x16
#define CMD_INITIALIZE_ADC        0x17
#define CMD_ENABLE_ADC_RUN        0x18
#define CMD_DISABLE_ADC_RUN       0x19
#define CMD_READ_SINGLE_CURRENT   0x1A
#define CMD_READ_ALL_CURRENT      0x1B
#define CMD_RECORD_ADC_OFFSET     0x1C
#define CMD_RETRIEVE_ADC_OFFSET	  0x1D

#define CMD_TURN_ON_HV          0x1E
#define CMD_TURN_OFF_HV         0x1F
#define CMD_ENABLE_HV_CTR       0x20
#define CMD_CHECK_HV_STATUS     0x21
#define CMD_SET_MAX_CURRENT	0x22

#define CMD_UPDATE_MUSIC_CONFIG		0x23
#define CMD_SET_BIAS_VOLTAGE		0x24
#define CMD_SET_TRG_THRESHOLD		0x25