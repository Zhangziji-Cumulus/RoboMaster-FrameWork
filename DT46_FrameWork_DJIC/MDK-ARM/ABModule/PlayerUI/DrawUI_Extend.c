#include "DrawUI_Extend.h"

/**
 * @brief 线段绕自身起始点旋转，计算旋转后线段末端坐标
 * @param x0 线段起始点X坐标，fp32(float)类型
 * @param y0 线段起始点Y坐标，fp32(float)类型
 * @param len 线段固定长度，浮点值，必须大于0
 * @param angle 外部输入旋转偏移量，单位：弧度 rad
 *              注意：该参数直接传入弧度，无需内部转换
 * @param base_angle_deg 基准初始偏移角，单位：度 °
 *        基准规则：以数学标准坐标系【水平向右为0°】为基准
 *        示例：0.0f = 线段初始水平向右；90.0f = 线段初始竖直向上；180.0f = 水平向左
 * @param reverse_dir 旋转方向反转控制标志，uint8_t单字节标志位
 *        0 = 标准旋转规则：正弧度逆时针旋转，负弧度顺时针旋转
 *        1 = 反转旋转规则：正弧度顺时针旋转(视觉向左摆)，负弧度逆时针旋转(视觉向右摆)
 * @param out_x 指针输出：旋转完成后线段末端X坐标，fp32(float)
 * @param out_y 指针输出：旋转完成后线段末端Y坐标，fp32(float)
 * @note 运算中间过程采用double双精度浮点计算，降低三角函数运算误差，最终强制转换为float输出适配UI结构体
 * @note 坐标系说明：默认数学坐标系Y轴向上；若为屏幕LCD绘图(Y轴向下)，需修改dy项符号
 * @example 需求：线段0°竖直向上，角度正数向左、负数向右
 *          RotateLineByStart(start_x, start_y, 100, Yaw_rad, 90.0f, 1, &out_x, &out_y);
 */
void RotateLineByStart(float x0, float y0, float len, double angle, float base_angle_deg, uint8_t reverse_dir, float *out_x, float *out_y)
{
    // 1. 将基准偏移角(角度°)转换为弧度，用于叠加
    double base_rad = DEG_TO_RAD((double)base_angle_deg);
    // 总弧度 = 基准弧度 + 外部输入弧度angle（angle本身已是rad，无需转换）
    double total_rad = base_rad + angle;

    double dx, dy;
    if (reverse_dir == 0)
    {
        // 标准旋转方向：正弧度逆时针，负弧度顺时针
        // cos(rad) 计算X轴方向长度分量，sin(rad)计算Y轴方向长度分量
        dx = len * cos(total_rad);
        dy = len * sin(total_rad);
    }
    else
    {
        // 反转旋转方向：等价于对总弧度取负，实现旋转方向翻转
        // 三角函数特性：cos(-rad) = cos(rad)，sin(-rad) = -sin(rad)
        dx = len * cos(-total_rad);
        dy = len * sin(-total_rad);
    }

    // 平移坐标系：将相对起点的偏移分量叠加到起始点坐标，得到绝对末端坐标
    // 强制转换double计算结果为float(fp32)，适配UI存储变量类型
    *out_x = x0 + (float)dx;
    *out_y = y0 + (float)dy;
}

/**
 * @brief 适配UI绘图场景的简化封装函数
 * @brief 固定配置：基准90°(零位线段竖直向上)、旋转方向反转(正弧度向左，负弧度向右)
 * @param x0 线段起点X fp32
 * @param y0 线段起点Y fp32
 * @param len 线段长度
 * @param angle 输入旋转Yaw，单位：弧度 rad
 * @param out_x 输出末端X fp32
 * @param out_y 输出末端Y fp32
 * @note 直接替换原有调用，无需额外传入基准角与方向标志，简化业务代码
 */
void RotateLineUI(float x0, float y0, float len, double angle, float *out_x, float *out_y)
{
    RotateLineByStart(x0, y0, len, angle, 90.0f,0, out_x, out_y);
}

/**
 * @brief 适配UI绘图场景的简化封装函数
 * @brief 固定配置：基准0°(水平向左)、旋转方向反转(正弧度向左，负弧度向右)
 * @param x0 线段起点X fp32
 * @param y0 线段起点Y fp32
 * @param len 线段长度
 * @param angle 输入旋转Yaw，单位：弧度 rad
 * @param out_x 输出末端X fp32
 * @param out_y 输出末端Y fp32
 * @note 直接替换原有调用，无需额外传入基准角与方向标志，简化业务代码
 */
void PitchLineUI(float x0, float y0, float len, double angle, float *out_x, float *out_y)
{
    RotateLineByStart(x0, y0, len, angle, 180.0f,0, out_x, out_y);
}

/**
 * @brief 角度插值：根据起始角度、角度范围、百分比计算终点角度
 * @param start_deg 初始角度 0~360 float
 * @param range_deg 总旋转角度范围，正数顺时针，负数逆时针
 * @param percent 百分比 0~100，支持超100或小于0
 * @param normalize 1：结果限制在 [0, 360)；0：不做角度归一
 * @return 目标末位置角度
 */
float LerpAngle(float start_deg, float range_deg, float percent, int normalize)
{
    // 百分比转为0~1系数
    float scale = percent / 100.0f;
    float end = start_deg + range_deg * scale;

    // 需要归一化时，内置取模处理
    if (normalize)
    {
        end = fmodf(end, 360.0f);
        if (end < 0.0f)
        {
            end += 360.0f;
        }
    }

    // 防止结果与起始角完全重合（0%时start_deg == end），避免弧线绘制API将0°弧误判为整圆
    // 注意：必须向减小的方向偏移，使弧线走向与正常充满方向一致且极小，否则会绕大圈359°几乎又是整圆
    if (fabsf(end - start_deg) < 0.001f)
    {
        end = start_deg - 0.001f;
        if (normalize)
        {
            if (end < 0.0f)      end += 360.0f;
            else if (end >= 360.0f) end -= 360.0f;
        }
    }

    return end;
}
