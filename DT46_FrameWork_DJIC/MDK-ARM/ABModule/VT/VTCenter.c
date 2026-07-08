#include "VTCenter.h"
#include "sound_effects_task.h"

#if(BOARD_ID == GIMBAL_BOARD)

//========= VT03 实例资源 =========
static uint8_t vt03_rx_buf[64];
static VT03_Data_t vt03_data;
static VT_Instance_t vt03_inst;

const VT_Config_t VT03_CFG = {
    .frame_len = 21,
    .rx_buf_len = 64,
    .head0 = 0xA9,
    .head1 = 0x53,
    .scan_interval_ms = 10,
    .signal_lost_ms = 100,
    .power_lost_ms = 500,
    .crc_fail_max = 3,
    .crc_ok_offset   = offsetof(VT03_Data_t, crc_ok),
    .is_valid_offset = offsetof(VT03_Data_t, is_valid)
};

const VT03_Data_t* get_VT03_Ctl_point(void)
{
    return &vt03_data;
}

static UBaseType_t remain_VTTask;
__attribute__((used)) void VTTask(void *argument)
{
    // 初始化串口6，huart6由CubeMX生成
    buzzer_t *buzzer = get_buzzer_effect_point();
    VT_Init(&vt03_inst, &huart6, &VT03_CFG, VT03_ParseCallback, vt03_rx_buf, &vt03_data);

    for(;;)
    {

        VT_Process(&vt03_inst);
        VT03_Data_t *rc = VT_GET_DATA(VT03_Data_t, &vt03_inst);

        //测试键盘功能是否正常
        
        // if(VT03_KeyTest(&vt03_data) || VT03_MouseTest(&vt03_data))
        // {
        //     buzzer->sound_effect = B_;
        // }

        // 栈余量监控
        remain_VTTask = uxTaskGetStackHighWaterMark(NULL);
        osDelay(1);
    }
}

#endif

