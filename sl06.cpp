#include "sl06.h"

using namespace pxt;
using namespace SL06;

//MicroBitI2C i2c(I2C_SDA0, I2C_SCL0);
static bool wireWriteByte(uint8_t val);
static bool wireWriteDataByte(uint8_t reg, uint8_t val);
static bool wireWriteDataBlock(uint8_t reg, uint8_t *val, unsigned int len);
static bool wireReadDataByte(uint8_t reg, uint8_t &val);
static int wireReadDataBlock(uint8_t reg, uint8_t *val, unsigned int len);
static void delay(uint16_t time);

xSL06::xSL06()
{
    APDS9960_I2C_ADDR = 0x72;

    gesture_ud_delta_ = 0;
    gesture_lr_delta_ = 0;

    gesture_ud_count_ = 0;
    gesture_lr_count_ = 0;

    gesture_near_count_ = 0;
    gesture_far_count_ = 0;

    gesture_state_ = 0;
    gesture_motion_ = DIR_NONE;
}

xSL06::~xSL06()
{
}

bool xSL06::begin()
{
    uint8_t id;

    /* Read ID register and check against known values for APDS-9960 */
    if (!wireReadDataByte(APDS9960_ID, id))
    {
        return false;
    }

    if (!(id == APDS9960_ID_1 || id == APDS9960_ID_2))
    {
        return false;
    }

    /* Set ENABLE register to 0 (disable all features) */
    if (!setMode(ALL, OFF))
    {
        return false;
    }

    /* Set default values for ambient light and proximity registers */
    if (!wireWriteDataByte(APDS9960_ATIME, DEFAULT_ATIME))
    {
        return false;
    }
    if (!wireWriteDataByte(APDS9960_WTIME, DEFAULT_WTIME))
    {
        return false;
    }
    if (!wireWriteDataByte(APDS9960_PPULSE, DEFAULT_PROX_PPULSE))
    {
        return false;
    }
    if (!wireWriteDataByte(APDS9960_POFFSET_UR, DEFAULT_POFFSET_UR))
    {
        return false;
    }
    if (!wireWriteDataByte(APDS9960_POFFSET_DL, DEFAULT_POFFSET_DL))
    {
        return false;
    }
    if (!wireWriteDataByte(APDS9960_CONFIG1, DEFAULT_CONFIG1))
    {
        return false;
    }
    if (!setLEDDrive(DEFAULT_LDRIVE))
    {
        return false;
    }
    if (!setProximityGain(DEFAULT_PGAIN))
    {
        return false;
    }
    if (!setAmbientLightGain(DEFAULT_AGAIN))
    {
        return false;
    }
    if (!setProxIntLowThresh(DEFAULT_PILT))
    {
        return false;
    }
    if (!setProxIntHighThresh(DEFAULT_PIHT))
    {
        return false;
    }
    if (!setLightIntLowThreshold(DEFAULT_AILT))
    {
        return false;
    }
    if (!setLightIntHighThreshold(DEFAULT_AIHT))
    {
        return false;
    }
    if (!wireWriteDataByte(APDS9960_PERS, DEFAULT_PERS))
    {
        return false;
    }
    if (!wireWriteDataByte(APDS9960_CONFIG2, DEFAULT_CONFIG2))
    {
        return false;
    }
    if (!wireWriteDataByte(APDS9960_CONFIG3, DEFAULT_CONFIG3))
    {
        return false;
    }

    /* Set default values for gesture sense registers */
    if (!setGestureEnterThresh(DEFAULT_GPENTH))
    {
        return false;
    }
    if (!setGestureExitThresh(DEFAULT_GEXTH))
    {
        return false;
    }
    if (!wireWriteDataByte(APDS9960_GCONF1, DEFAULT_GCONF1))
    {
        return false;
    }
    if (!setGestureGain(DEFAULT_GGAIN))
    {
        return false;
    }
    if (!setGestureLEDDrive(DEFAULT_GLDRIVE))
    {
        return false;
    }
    if (!setGestureWaitTime(DEFAULT_GWTIME))
    {
        return false;
    }
    if (!wireWriteDataByte(APDS9960_GOFFSET_U, DEFAULT_GOFFSET))
    {
        return false;
    }
    if (!wireWriteDataByte(APDS9960_GOFFSET_D, DEFAULT_GOFFSET))
    {
        return false;
    }
    if (!wireWriteDataByte(APDS9960_GOFFSET_L, DEFAULT_GOFFSET))
    {
        return false;
    }
    if (!wireWriteDataByte(APDS9960_GOFFSET_R, DEFAULT_GOFFSET))
    {
        return false;
    }
    if (!wireWriteDataByte(APDS9960_GPULSE, DEFAULT_GPULSE))
    {
        return false;
    }
    if (!wireWriteDataByte(APDS9960_GCONF3, DEFAULT_GCONF3))
    {
        return false;
    }
    if (!setGestureIntEnable(DEFAULT_GIEN))
    {
        return false;
    }

#if 0
    /* Gesture config register dump */
    uint8_t reg;
    uint8_t val;
  
    for(reg = 0x80; reg <= 0xAF; reg++) {
        if( (reg != 0x82) && \
            (reg != 0x8A) && \
            (reg != 0x91) && \
            (reg != 0xA8) && \
            (reg != 0xAC) && \
            (reg != 0xAD) )
        {
            wireReadDataByte(reg, val);
        }
    }

    for(reg = 0xE4; reg <= 0xE7; reg++) {
        wireReadDataByte(reg, val);
    }
#endif

    return true;
}

uint8_t xSL06::getMode()
{
    uint8_t enable_value;

    /* Read current ENABLE register */
    if (!wireReadDataByte(APDS9960_ENABLE, enable_value))
    {
        return ERROR;
    }

    return enable_value;
}

bool xSL06::setMode(uint8_t mode, uint8_t enable)
{
    uint8_t reg_val;

    /* Read current ENABLE register */
    reg_val = getMode();
    if (reg_val == ERROR)
    {
        return false;
    }

    /* Change bit(s) in ENABLE register */
    enable = enable & 0x01;
    if (mode >= 0 && mode <= 6)
    {
        if (enable)
        {
            reg_val |= (1 << mode);
        }
        else
        {
            reg_val &= ~(1 << mode);
        }
    }
    else if (mode == ALL)
    {
        if (enable)
        {
            reg_val = 0x7F;
        }
        else
        {
            reg_val = 0x00;
        }
    }

    /* Write value back to ENABLE register */
    if (!wireWriteDataByte(APDS9960_ENABLE, reg_val))
    {
        return false;
    }

    return true;
}

bool xSL06::enableLightSensor(bool interrupts)
{

    /* Set default gain, interrupts, enable power, and enable sensor */
    if (!setAmbientLightGain(DEFAULT_AGAIN))
    {
        return false;
    }
    if (interrupts)
    {
        if (!setAmbientLightIntEnable(1))
        {
            return false;
        }
    }
    else
    {
        if (!setAmbientLightIntEnable(0))
        {
            return false;
        }
    }
    if (!enablePower())
    {
        return false;
    }
    if (!setMode(AMBIENT_LIGHT, 1))
    {
        return false;
    }

    return true;
}

bool xSL06::disableLightSensor()
{
    if (!setAmbientLightIntEnable(0))
    {
        return false;
    }
    if (!setMode(AMBIENT_LIGHT, 0))
    {
        return false;
    }

    return true;
}

bool xSL06::enableProximitySensor(bool interrupts)
{
    /* Set default gain, LED, interrupts, enable power, and enable sensor */
    if (!setProximityGain(DEFAULT_PGAIN))
    {
        return false;
    }
    if (!setLEDDrive(DEFAULT_LDRIVE))
    {
        return false;
    }
    if (interrupts)
    {
        if (!setProximityIntEnable(1))
        {
            return false;
        }
    }
    else
    {
        if (!setProximityIntEnable(0))
        {
            return false;
        }
    }
    if (!enablePower())
    {
        return false;
    }
    if (!setMode(PROXIMITY, 1))
    {
        return false;
    }

    return true;
}

bool xSL06::disableProximitySensor()
{
    if (!setProximityIntEnable(0))
    {
        return false;
    }
    if (!setMode(PROXIMITY, 0))
    {
        return false;
    }

    return true;
}

bool xSL06::enableGestureSensor(bool interrupts)
{

    /* Enable gesture mode
       Set ENABLE to 0 (power off)
       Set WTIME to 0xFF
       Set AUX to LED_BOOST_300
       Enable PON, WEN, PEN, GEN in ENABLE 
    */
    resetGestureParameters();
    if (!wireWriteDataByte(APDS9960_WTIME, 0xFF))
    {
        return false;
    }
    if (!wireWriteDataByte(APDS9960_PPULSE, DEFAULT_GESTURE_PPULSE))
    {
        return false;
    }
    if (!setLEDBoost(LED_BOOST_300))
    {
        return false;
    }
    if (interrupts)
    {
        if (!setGestureIntEnable(1))
        {
            return false;
        }
    }
    else
    {
        if (!setGestureIntEnable(0))
        {
            return false;
        }
    }
    if (!setGestureMode(1))
    {
        return false;
    }
    if (!enablePower())
    {
        return false;
    }
    if (!setMode(WAIT, 1))
    {
        return false;
    }
    if (!setMode(PROXIMITY, 1))
    {
        return false;
    }
    if (!setMode(GESTURE, 1))
    {
        return false;
    }

    return true;
}

bool xSL06::disableGestureSensor()
{
    resetGestureParameters();
    if (!setGestureIntEnable(0))
    {
        return false;
    }
    if (!setGestureMode(0))
    {
        return false;
    }
    if (!setMode(GESTURE, 0))
    {
        return false;
    }

    return true;
}

bool xSL06::isGestureAvailable()
{
    uint8_t val;

    /* Read value from GSTATUS register */
    if (!wireReadDataByte(APDS9960_GSTATUS, val))
    {
        return ERROR;
    }

    /* Shift and mask out GVALID bit */
    val &= APDS9960_GVALID;

    /* Return true/false based on GVALID bit */
    if (val == 1)
    {
        return true;
    }
    else
    {
        return false;
    }
}

int xSL06::getGesture()
{
    uint8_t fifo_level = 0;
    uint8_t bytes_read = 0;
    uint8_t fifo_data[128];
    uint8_t gstatus;
    int motion;
    int i;

    /* Make sure that power and gesture is on and data is valid */
    if (!isGestureAvailable() || !(getMode() & 0b01000001))
    {
        return DIR_NONE;
    }

    /* Keep looping as long as gesture data is valid */
    while (1)
    {

        /* Wait some time to collect next batch of FIFO data */
        delay(FIFO_PAUSE_TIME);

        /* Get the contents of the STATUS register. Is data still valid? */
        if (!wireReadDataByte(APDS9960_GSTATUS, gstatus))
        {
            return ERROR;
        }

        /* If we have valid data, read in FIFO */
        if ((gstatus & APDS9960_GVALID) == APDS9960_GVALID)
        {

            /* Read the current FIFO level */
            if (!wireReadDataByte(APDS9960_GFLVL, fifo_level))
            {
                return ERROR;
            }

            /* If there's stuff in the FIFO, read it into our data block */
            if (fifo_level > 0)
            {
                bytes_read = wireReadDataBlock(APDS9960_GFIFO_U,
                                               (uint8_t *)fifo_data,
                                               (fifo_level * 4));
                if (bytes_read == -1)
                {
                    return ERROR;
                }

                /* If at least 1 set of data, sort the data into U/D/L/R */
                if (bytes_read >= 4)
                {
                    for (i = 0; i < bytes_read; i += 4)
                    {
                        gesture_data_.u_data[gesture_data_.index] =
                            fifo_data[i + 0];
                        gesture_data_.d_data[gesture_data_.index] =
                            fifo_data[i + 1];
                        gesture_data_.l_data[gesture_data_.index] =
                            fifo_data[i + 2];
                        gesture_data_.r_data[gesture_data_.index] =
                            fifo_data[i + 3];
                        gesture_data_.index++;
                        gesture_data_.total_gestures++;
                    }

                    /* Filter and process gesture data. Decode near/far state */
                    if (processGestureData())
                    {
                        if (decodeGesture())
                        {
                            //***TODO: U-Turn Gestures
                        }
                    }

                    /* Reset data */
                    gesture_data_.index = 0;
                    gesture_data_.total_gestures = 0;
                }
            }
        }
        else
        {

            /* Determine best guessed gesture and clean up */
            delay(FIFO_PAUSE_TIME);
            decodeGesture();
            motion = gesture_motion_;

            resetGestureParameters();
            return motion;
        }
    }
}

bool xSL06::enablePower()
{
    if (!setMode(POWER, 1))
    {
        return false;
    }

    return true;
}

bool xSL06::disablePower()
{
    if (!setMode(POWER, 0))
    {
        return false;
    }

    return true;
}

uint16_t xSL06::getAmbientLight()
{
    uint8_t val_byte;
    uint16_t val = 0;

    /* Read value from clear channel, low byte register */
    if (!wireReadDataByte(APDS9960_CDATAL, val_byte))
    {
        return ERROR;
    }
    val = val_byte;

    /* Read value from clear channel, high byte register */
    if (!wireReadDataByte(APDS9960_CDATAH, val_byte))
    {
        return ERROR;
    }
    val = val + ((uint16_t)val_byte << 8);

    return val;
}

uint16_t xSL06::getRedLight()
{
    uint8_t val_byte;
    uint16_t val = 0;

    /* Read value from clear channel, low byte register */
    if (!wireReadDataByte(APDS9960_RDATAL, val_byte))
    {
        return ERROR;
    }
    val = val_byte;

    /* Read value from clear channel, high byte register */
    if (!wireReadDataByte(APDS9960_RDATAH, val_byte))
    {
        return ERROR;
    }
    val = val + ((uint16_t)val_byte << 8);

    return val;
}

uint16_t xSL06::getGreenLight()
{
    uint8_t val_byte;
    uint16_t val = 0;

    /* Read value from clear channel, low byte register */
    if (!wireReadDataByte(APDS9960_GDATAL, val_byte))
    {
        return ERROR;
    }
    val = val_byte;

    /* Read value from clear channel, high byte register */
    if (!wireReadDataByte(APDS9960_GDATAH, val_byte))
    {
        return ERROR;
    }
    val = val + ((uint16_t)val_byte << 8);

    return val;
}

uint16_t xSL06::getBlueLight()
{
    uint8_t val_byte;
    uint16_t val = 0;

    /* Read value from clear channel, low byte register */
    if (!wireReadDataByte(APDS9960_BDATAL, val_byte))
    {
        return ERROR;
    }
    val = val_byte;

    /* Read value from clear channel, high byte register */
    if (!wireReadDataByte(APDS9960_BDATAH, val_byte))
    {
        return ERROR;
    }
    val = val + ((uint16_t)val_byte << 8);

    return val;
}

uint8_t xSL06::getProximity(void)
{
    uint8_t val;
    //val=0;

    /* Read value from proximity data register */
    if (!wireReadDataByte(APDS9960_PDATA, val))
    {
        return ERROR;
    }

    return val;
}

void xSL06::resetGestureParameters()
{
    gesture_data_.index = 0;
    gesture_data_.total_gestures = 0;

    gesture_ud_delta_ = 0;
    gesture_lr_delta_ = 0;

    gesture_ud_count_ = 0;
    gesture_lr_count_ = 0;

    gesture_near_count_ = 0;
    gesture_far_count_ = 0;

    gesture_state_ = 0;
    gesture_motion_ = DIR_NONE;
}

bool xSL06::processGestureData()
{
    uint8_t u_first = 0;
    uint8_t d_first = 0;
    uint8_t l_first = 0;
    uint8_t r_first = 0;
    uint8_t u_last = 0;
    uint8_t d_last = 0;
    uint8_t l_last = 0;
    uint8_t r_last = 0;
    int ud_ratio_first;
    int lr_ratio_first;
    int ud_ratio_last;
    int lr_ratio_last;
    int ud_delta;
    int lr_delta;
    int i;

    /* If we have less than 4 total gestures, that's not enough */
    if (gesture_data_.total_gestures <= 4)
    {
        return false;
    }

    /* Check to make sure our data isn't out of bounds */
    if ((gesture_data_.total_gestures <= 32) &&
        (gesture_data_.total_gestures > 0))
    {

        /* Find the first value in U/D/L/R above the threshold */
        for (i = 0; i < gesture_data_.total_gestures; i++)
        {
            if ((gesture_data_.u_data[i] > GESTURE_THRESHOLD_OUT) &&
                (gesture_data_.d_data[i] > GESTURE_THRESHOLD_OUT) &&
                (gesture_data_.l_data[i] > GESTURE_THRESHOLD_OUT) &&
                (gesture_data_.r_data[i] > GESTURE_THRESHOLD_OUT))
            {

                u_first = gesture_data_.u_data[i];
                d_first = gesture_data_.d_data[i];
                l_first = gesture_data_.l_data[i];
                r_first = gesture_data_.r_data[i];
                break;
            }
        }

        /* If one of the _first values is 0, then there is no good data */
        if ((u_first == 0) || (d_first == 0) ||
            (l_first == 0) || (r_first == 0))
        {

            return false;
        }
        /* Find the last value in U/D/L/R above the threshold */
        for (i = gesture_data_.total_gestures - 1; i >= 0; i--)
        {

            if ((gesture_data_.u_data[i] > GESTURE_THRESHOLD_OUT) &&
                (gesture_data_.d_data[i] > GESTURE_THRESHOLD_OUT) &&
                (gesture_data_.l_data[i] > GESTURE_THRESHOLD_OUT) &&
                (gesture_data_.r_data[i] > GESTURE_THRESHOLD_OUT))
            {

                u_last = gesture_data_.u_data[i];
                d_last = gesture_data_.d_data[i];
                l_last = gesture_data_.l_data[i];
                r_last = gesture_data_.r_data[i];
                break;
            }
        }
    }

    /* Calculate the first vs. last ratio of up/down and left/right */
    ud_ratio_first = ((u_first - d_first) * 100) / (u_first + d_first);
    lr_ratio_first = ((l_first - r_first) * 100) / (l_first + r_first);
    ud_ratio_last = ((u_last - d_last) * 100) / (u_last + d_last);
    lr_ratio_last = ((l_last - r_last) * 100) / (l_last + r_last);

    /* Determine the difference between the first and last ratios */
    ud_delta = ud_ratio_last - ud_ratio_first;
    lr_delta = lr_ratio_last - lr_ratio_first;

    /* Accumulate the UD and LR delta values */
    gesture_ud_delta_ += ud_delta;
    gesture_lr_delta_ += lr_delta;

    /* Determine U/D gesture */
    if (gesture_ud_delta_ >= GESTURE_SENSITIVITY_1)
    {
        gesture_ud_count_ = 1;
    }
    else if (gesture_ud_delta_ <= -GESTURE_SENSITIVITY_1)
    {
        gesture_ud_count_ = -1;
    }
    else
    {
        gesture_ud_count_ = 0;
    }

    /* Determine L/R gesture */
    if (gesture_lr_delta_ >= GESTURE_SENSITIVITY_1)
    {
        gesture_lr_count_ = 1;
    }
    else if (gesture_lr_delta_ <= -GESTURE_SENSITIVITY_1)
    {
        gesture_lr_count_ = -1;
    }
    else
    {
        gesture_lr_count_ = 0;
    }

    /* Determine Near/Far gesture */
    if ((gesture_ud_count_ == 0) && (gesture_lr_count_ == 0))
    {
        if ((abs(ud_delta) < GESTURE_SENSITIVITY_2) &&
            (abs(lr_delta) < GESTURE_SENSITIVITY_2))
        {

            if ((ud_delta == 0) && (lr_delta == 0))
            {
                gesture_near_count_++;
            }
            else if ((ud_delta != 0) || (lr_delta != 0))
            {
                gesture_far_count_++;
            }

            if ((gesture_near_count_ >= 10) && (gesture_far_count_ >= 2))
            {
                if ((ud_delta == 0) && (lr_delta == 0))
                {
                    gesture_state_ = NEAR_STATE1;
                }
                else if ((ud_delta != 0) && (lr_delta != 0))
                {
                    gesture_state_ = FAR_STATE1;
                }
                return true;
            }
        }
    }
    else
    {
        if ((abs(ud_delta) < GESTURE_SENSITIVITY_2) &&
            (abs(lr_delta) < GESTURE_SENSITIVITY_2))
        {

            if ((ud_delta == 0) && (lr_delta == 0))
            {
                gesture_near_count_++;
            }

            if (gesture_near_count_ >= 10)
            {
                gesture_ud_count_ = 0;
                gesture_lr_count_ = 0;
                gesture_ud_delta_ = 0;
                gesture_lr_delta_ = 0;
            }
        }
    }
    return false;
}

bool xSL06::decodeGesture()
{
    /* Return if near or far event is detected */
    if (gesture_state_ == NEAR_STATE1)
    {
        gesture_motion_ = DIR_NEAR;
        return true;
    }
    else if (gesture_state_ == FAR_STATE1)
    {
        gesture_motion_ = DIR_FAR;
        return true;
    }

    /* Determine swipe direction */
    if ((gesture_ud_count_ == -1) && (gesture_lr_count_ == 0))
    {
        gesture_motion_ = DIR_UP;
    }
    else if ((gesture_ud_count_ == 1) && (gesture_lr_count_ == 0))
    {
        gesture_motion_ = DIR_DOWN;
    }
    else if ((gesture_ud_count_ == 0) && (gesture_lr_count_ == 1))
    {
        gesture_motion_ = DIR_RIGHT;
    }
    else if ((gesture_ud_count_ == 0) && (gesture_lr_count_ == -1))
    {
        gesture_motion_ = DIR_LEFT;
    }
    else if ((gesture_ud_count_ == -1) && (gesture_lr_count_ == 1))
    {
        if (abs(gesture_ud_delta_) > abs(gesture_lr_delta_))
        {
            gesture_motion_ = DIR_UP;
        }
        else
        {
            gesture_motion_ = DIR_RIGHT;
        }
    }
    else if ((gesture_ud_count_ == 1) && (gesture_lr_count_ == -1))
    {
        if (abs(gesture_ud_delta_) > abs(gesture_lr_delta_))
        {
            gesture_motion_ = DIR_DOWN;
        }
        else
        {
            gesture_motion_ = DIR_LEFT;
        }
    }
    else if ((gesture_ud_count_ == -1) && (gesture_lr_count_ == -1))
    {
        if (abs(gesture_ud_delta_) > abs(gesture_lr_delta_))
        {
            gesture_motion_ = DIR_UP;
        }
        else
        {
            gesture_motion_ = DIR_LEFT;
        }
    }
    else if ((gesture_ud_count_ == 1) && (gesture_lr_count_ == 1))
    {
        if (abs(gesture_ud_delta_) > abs(gesture_lr_delta_))
        {
            gesture_motion_ = DIR_DOWN;
        }
        else
        {
            gesture_motion_ = DIR_RIGHT;
        }
    }
    else
    {
        return false;
    }

    return true;
}

uint8_t xSL06::getProxIntLowThresh()
{
    uint8_t val;

    /* Read value from PILT register */
    if (!wireReadDataByte(APDS9960_PILT, val))
    {
        val = 0;
    }

    return val;
}

bool xSL06::setProxIntLowThresh(uint8_t threshold)
{
    if (!wireWriteDataByte(APDS9960_PILT, threshold))
    {
        return false;
    }

    return true;
}

uint8_t xSL06::getProxIntHighThresh()
{
    uint8_t val;

    /* Read value from PIHT register */
    if (!wireReadDataByte(APDS9960_PIHT, val))
    {
        val = 0;
    }

    return val;
}

bool xSL06::setProxIntHighThresh(uint8_t threshold)
{
    if (!wireWriteDataByte(APDS9960_PIHT, threshold))
    {
        return false;
    }

    return true;
}

uint8_t xSL06::getLEDDrive()
{
    uint8_t val;

    /* Read value from CONTROL register */
    if (!wireReadDataByte(APDS9960_CONTROL, val))
    {
        return ERROR;
    }

    /* Shift and mask out LED drive bits */
    val = (val >> 6) & 0b00000011;

    return val;
}

bool xSL06::setLEDDrive(uint8_t drive)
{
    uint8_t val;

    /* Read value from CONTROL register */
    if (!wireReadDataByte(APDS9960_CONTROL, val))
    {
        return false;
    }

    /* Set bits in register to given value */
    drive &= 0b00000011;
    drive = drive << 6;
    val &= 0b00111111;
    val |= drive;

    /* Write register value back into CONTROL register */
    if (!wireWriteDataByte(APDS9960_CONTROL, val))
    {
        return false;
    }

    return true;
}

uint8_t xSL06::getProximityGain()
{
    uint8_t val;

    /* Read value from CONTROL register */
    if (!wireReadDataByte(APDS9960_CONTROL, val))
    {
        return ERROR;
    }

    /* Shift and mask out PDRIVE bits */
    val = (val >> 2) & 0b00000011;

    return val;
}

bool xSL06::setProximityGain(uint8_t drive)
{
    uint8_t val;

    /* Read value from CONTROL register */
    if (!wireReadDataByte(APDS9960_CONTROL, val))
    {
        return false;
    }

    /* Set bits in register to given value */
    drive &= 0b00000011;
    drive = drive << 2;
    val &= 0b11110011;
    val |= drive;

    /* Write register value back into CONTROL register */
    if (!wireWriteDataByte(APDS9960_CONTROL, val))
    {
        return false;
    }

    return true;
}

uint8_t xSL06::getAmbientLightGain()
{
    uint8_t val;

    /* Read value from CONTROL register */
    if (!wireReadDataByte(APDS9960_CONTROL, val))
    {
        return ERROR;
    }

    /* Shift and mask out ADRIVE bits */
    val &= 0b00000011;

    return val;
}

bool xSL06::setAmbientLightGain(uint8_t drive)
{
    uint8_t val;

    /* Read value from CONTROL register */
    if (!wireReadDataByte(APDS9960_CONTROL, val))
    {
        return false;
    }

    /* Set bits in register to given value */
    drive &= 0b00000011;
    val &= 0b11111100;
    val |= drive;

    /* Write register value back into CONTROL register */
    if (!wireWriteDataByte(APDS9960_CONTROL, val))
    {
        return false;
    }

    return true;
}

uint8_t xSL06::getLEDBoost()
{
    uint8_t val;

    /* Read value from CONFIG2 register */
    if (!wireReadDataByte(APDS9960_CONFIG2, val))
    {
        return ERROR;
    }

    /* Shift and mask out LED_BOOST bits */
    val = (val >> 4) & 0b00000011;

    return val;
}

bool xSL06::setLEDBoost(uint8_t boost)
{
    uint8_t val;

    /* Read value from CONFIG2 register */
    if (!wireReadDataByte(APDS9960_CONFIG2, val))
    {
        return false;
    }

    /* Set bits in register to given value */
    boost &= 0b00000011;
    boost = boost << 4;
    val &= 0b11001111;
    val |= boost;

    /* Write register value back into CONFIG2 register */
    if (!wireWriteDataByte(APDS9960_CONFIG2, val))
    {
        return false;
    }

    return true;
}

uint8_t xSL06::getProxGainCompEnable()
{
    uint8_t val;

    /* Read value from CONFIG3 register */
    if (!wireReadDataByte(APDS9960_CONFIG3, val))
    {
        return ERROR;
    }

    /* Shift and mask out PCMP bits */
    val = (val >> 5) & 0b00000001;

    return val;
}

bool xSL06::setProxGainCompEnable(uint8_t enable)
{
    uint8_t val;

    /* Read value from CONFIG3 register */
    if (!wireReadDataByte(APDS9960_CONFIG3, val))
    {
        return false;
    }

    /* Set bits in register to given value */
    enable &= 0b00000001;
    enable = enable << 5;
    val &= 0b11011111;
    val |= enable;

    /* Write register value back into CONFIG3 register */
    if (!wireWriteDataByte(APDS9960_CONFIG3, val))
    {
        return false;
    }

    return true;
}

uint8_t xSL06::getProxPhotoMask()
{
    uint8_t val;

    /* Read value from CONFIG3 register */
    if (!wireReadDataByte(APDS9960_CONFIG3, val))
    {
        return ERROR;
    }

    /* Mask out photodiode enable mask bits */
    val &= 0b00001111;

    return val;
}

bool xSL06::setProxPhotoMask(uint8_t mask)
{
    uint8_t val;

    /* Read value from CONFIG3 register */
    if (!wireReadDataByte(APDS9960_CONFIG3, val))
    {
        return false;
    }

    /* Set bits in register to given value */
    mask &= 0b00001111;
    val &= 0b11110000;
    val |= mask;

    /* Write register value back into CONFIG3 register */
    if (!wireWriteDataByte(APDS9960_CONFIG3, val))
    {
        return false;
    }

    return true;
}

uint8_t xSL06::getGestureEnterThresh()
{
    uint8_t val;

    /* Read value from GPENTH register */
    if (!wireReadDataByte(APDS9960_GPENTH, val))
    {
        val = 0;
    }

    return val;
}

bool xSL06::setGestureEnterThresh(uint8_t threshold)
{
    if (!wireWriteDataByte(APDS9960_GPENTH, threshold))
    {
        return false;
    }

    return true;
}

uint8_t xSL06::getGestureExitThresh()
{
    uint8_t val;

    /* Read value from GEXTH register */
    if (!wireReadDataByte(APDS9960_GEXTH, val))
    {
        val = 0;
    }

    return val;
}

bool xSL06::setGestureExitThresh(uint8_t threshold)
{
    if (!wireWriteDataByte(APDS9960_GEXTH, threshold))
    {
        return false;
    }

    return true;
}

uint8_t xSL06::getGestureGain()
{
    uint8_t val;

    /* Read value from GCONF2 register */
    if (!wireReadDataByte(APDS9960_GCONF2, val))
    {
        return ERROR;
    }

    /* Shift and mask out GGAIN bits */
    val = (val >> 5) & 0b00000011;

    return val;
}

bool xSL06::setGestureGain(uint8_t gain)
{
    uint8_t val;

    /* Read value from GCONF2 register */
    if (!wireReadDataByte(APDS9960_GCONF2, val))
    {
        return false;
    }

    /* Set bits in register to given value */
    gain &= 0b00000011;
    gain = gain << 5;
    val &= 0b10011111;
    val |= gain;

    /* Write register value back into GCONF2 register */
    if (!wireWriteDataByte(APDS9960_GCONF2, val))
    {
        return false;
    }

    return true;
}

uint8_t xSL06::getGestureLEDDrive()
{
    uint8_t val;

    /* Read value from GCONF2 register */
    if (!wireReadDataByte(APDS9960_GCONF2, val))
    {
        return ERROR;
    }

    /* Shift and mask out GLDRIVE bits */
    val = (val >> 3) & 0b00000011;

    return val;
}

bool xSL06::setGestureLEDDrive(uint8_t drive)
{
    uint8_t val;

    /* Read value from GCONF2 register */
    if (!wireReadDataByte(APDS9960_GCONF2, val))
    {
        return false;
    }

    /* Set bits in register to given value */
    drive &= 0b00000011;
    drive = drive << 3;
    val &= 0b11100111;
    val |= drive;

    /* Write register value back into GCONF2 register */
    if (!wireWriteDataByte(APDS9960_GCONF2, val))
    {
        return false;
    }

    return true;
}

uint8_t xSL06::getGestureWaitTime()
{
    uint8_t val;

    /* Read value from GCONF2 register */
    if (!wireReadDataByte(APDS9960_GCONF2, val))
    {
        return ERROR;
    }

    /* Mask out GWTIME bits */
    val &= 0b00000111;

    return val;
}

bool xSL06::setGestureWaitTime(uint8_t time)
{
    uint8_t val;

    /* Read value from GCONF2 register */
    if (!wireReadDataByte(APDS9960_GCONF2, val))
    {
        return false;
    }

    /* Set bits in register to given value */
    time &= 0b00000111;
    val &= 0b11111000;
    val |= time;

    /* Write register value back into GCONF2 register */
    if (!wireWriteDataByte(APDS9960_GCONF2, val))
    {
        return false;
    }

    return true;
}

bool xSL06::getLightIntLowThreshold(uint16_t &threshold)
{
    uint8_t val_byte;
    threshold = 0;

    /* Read value from ambient light low threshold, low byte register */
    if (!wireReadDataByte(APDS9960_AILTL, val_byte))
    {
        return false;
    }
    threshold = val_byte;

    /* Read value from ambient light low threshold, high byte register */
    if (!wireReadDataByte(APDS9960_AILTH, val_byte))
    {
        return false;
    }
    threshold = threshold + ((uint16_t)val_byte << 8);

    return true;
}

bool xSL06::setLightIntLowThreshold(uint16_t threshold)
{
    uint8_t val_low;
    uint8_t val_high;

    /* Break 16-bit threshold into 2 8-bit values */
    val_low = threshold & 0x00FF;
    val_high = (threshold & 0xFF00) >> 8;

    /* Write low byte */
    if (!wireWriteDataByte(APDS9960_AILTL, val_low))
    {
        return false;
    }

    /* Write high byte */
    if (!wireWriteDataByte(APDS9960_AILTH, val_high))
    {
        return false;
    }

    return true;
}

bool xSL06::getLightIntHighThreshold(uint16_t &threshold)
{
    uint8_t val_byte;
    threshold = 0;

    /* Read value from ambient light high threshold, low byte register */
    if (!wireReadDataByte(APDS9960_AIHTL, val_byte))
    {
        return false;
    }
    threshold = val_byte;

    /* Read value from ambient light high threshold, high byte register */
    if (!wireReadDataByte(APDS9960_AIHTH, val_byte))
    {
        return false;
    }
    threshold = threshold + ((uint16_t)val_byte << 8);

    return true;
}

bool xSL06::setLightIntHighThreshold(uint16_t threshold)
{
    uint8_t val_low;
    uint8_t val_high;

    /* Break 16-bit threshold into 2 8-bit values */
    val_low = threshold & 0x00FF;
    val_high = (threshold & 0xFF00) >> 8;

    /* Write low byte */
    if (!wireWriteDataByte(APDS9960_AIHTL, val_low))
    {
        return false;
    }

    /* Write high byte */
    if (!wireWriteDataByte(APDS9960_AIHTH, val_high))
    {
        return false;
    }

    return true;
}

bool xSL06::getProximityIntLowThreshold(uint8_t &threshold)
{
    threshold = 0;

    /* Read value from proximity low threshold register */
    if (!wireReadDataByte(APDS9960_PILT, threshold))
    {
        return false;
    }

    return true;
}

bool xSL06::setProximityIntLowThreshold(uint8_t threshold)
{

    /* Write threshold value to register */
    if (!wireWriteDataByte(APDS9960_PILT, threshold))
    {
        return false;
    }

    return true;
}

bool xSL06::getProximityIntHighThreshold(uint8_t &threshold)
{
    threshold = 0;

    /* Read value from proximity low threshold register */
    if (!wireReadDataByte(APDS9960_PIHT, threshold))
    {
        return false;
    }

    return true;
}

bool xSL06::setProximityIntHighThreshold(uint8_t threshold)
{

    /* Write threshold value to register */
    if (!wireWriteDataByte(APDS9960_PIHT, threshold))
    {
        return false;
    }

    return true;
}

uint8_t xSL06::getAmbientLightIntEnable()
{
    uint8_t val;

    /* Read value from ENABLE register */
    if (!wireReadDataByte(APDS9960_ENABLE, val))
    {
        return ERROR;
    }

    /* Shift and mask out AIEN bit */
    val = (val >> 4) & 0b00000001;

    return val;
}

bool xSL06::setAmbientLightIntEnable(uint8_t enable)
{
    uint8_t val;

    /* Read value from ENABLE register */
    if (!wireReadDataByte(APDS9960_ENABLE, val))
    {
        return false;
    }

    /* Set bits in register to given value */
    enable &= 0b00000001;
    enable = enable << 4;
    val &= 0b11101111;
    val |= enable;

    /* Write register value back into ENABLE register */
    if (!wireWriteDataByte(APDS9960_ENABLE, val))
    {
        return false;
    }

    return true;
}

uint8_t xSL06::getProximityIntEnable()
{
    uint8_t val;

    /* Read value from ENABLE register */
    if (!wireReadDataByte(APDS9960_ENABLE, val))
    {
        return ERROR;
    }

    /* Shift and mask out PIEN bit */
    val = (val >> 5) & 0b00000001;

    return val;
}

bool xSL06::setProximityIntEnable(uint8_t enable)
{
    uint8_t val;

    /* Read value from ENABLE register */
    if (!wireReadDataByte(APDS9960_ENABLE, val))
    {
        return false;
    }

    /* Set bits in register to given value */
    enable &= 0b00000001;
    enable = enable << 5;
    val &= 0b11011111;
    val |= enable;

    /* Write register value back into ENABLE register */
    if (!wireWriteDataByte(APDS9960_ENABLE, val))
    {
        return false;
    }

    return true;
}

uint8_t xSL06::getGestureIntEnable()
{
    uint8_t val;

    /* Read value from GCONF4 register */
    if (!wireReadDataByte(APDS9960_GCONF4, val))
    {
        return ERROR;
    }

    /* Shift and mask out GIEN bit */
    val = (val >> 1) & 0b00000001;

    return val;
}

bool xSL06::setGestureIntEnable(uint8_t enable)
{
    uint8_t val;

    /* Read value from GCONF4 register */
    if (!wireReadDataByte(APDS9960_GCONF4, val))
    {
        return false;
    }

    /* Set bits in register to given value */
    enable &= 0b00000001;
    enable = enable << 1;
    val &= 0b11111101;
    val |= enable;

    /* Write register value back into GCONF4 register */
    if (!wireWriteDataByte(APDS9960_GCONF4, val))
    {
        return false;
    }

    return true;
}

bool xSL06::clearAmbientLightInt()
{
    uint8_t throwaway;
    if (!wireReadDataByte(APDS9960_AICLEAR, throwaway))
    {
        return false;
    }

    return true;
}

bool xSL06::clearProximityInt()
{
    uint8_t throwaway;
    if (!wireReadDataByte(APDS9960_PICLEAR, throwaway))
    {
        return false;
    }

    return true;
}

uint8_t xSL06::getGestureMode()
{
    uint8_t val;

    /* Read value from GCONF4 register */
    if (!wireReadDataByte(APDS9960_GCONF4, val))
    {
        return ERROR;
    }

    /* Mask out GMODE bit */
    val &= 0b00000001;

    return val;
}

bool xSL06::setGestureMode(uint8_t mode)
{
    uint8_t val;

    /* Read value from GCONF4 register */
    if (!wireReadDataByte(APDS9960_GCONF4, val))
    {
        return false;
    }

    /* Set bits in register to given value */
    mode &= 0b00000001;
    val &= 0b11111110;
    val |= mode;

    /* Write register value back into GCONF4 register */
    if (!wireWriteDataByte(APDS9960_GCONF4, val))
    {
        return false;
    }

    return true;
}

static bool wireWriteByte(uint8_t val)
{
    auto sda = LOOKUP_PIN(SDA);
    auto scl = LOOKUP_PIN(SCL);
    codal::I2C *i2c = pxt::getI2C(sda, scl);
    int error;
    error = i2c->write((uint16_t)APDS9960_I2C_ADDR, (uint8_t *)&val, 1);
    if (error != 0)
    {
        return false;
    }
    return true;
}

static bool wireWriteDataByte(uint8_t reg, uint8_t val)
{
    auto sda = LOOKUP_PIN(SDA);
    auto scl = LOOKUP_PIN(SCL);
    codal::I2C *i2c = pxt::getI2C(sda, scl);
    int error;
    uint8_t buf[2];
    buf[0] = (uint8_t)reg;
    buf[1] = (uint8_t)val;
    error = i2c->write((uint16_t)APDS9960_I2C_ADDR, (uint8_t *)&buf, 2);
    if (error != 0)
    {
        return false;
    }
    return true;
}

static bool wireWriteDataBlock(uint8_t reg, uint8_t *val, unsigned int len)
{
    return true;
}

static bool wireReadDataByte(uint8_t reg, uint8_t &val)
{
    auto sda = LOOKUP_PIN(SDA);
    auto scl = LOOKUP_PIN(SCL);
    codal::I2C *i2c = pxt::getI2C(sda, scl);
    int error;
    error = i2c->write((uint16_t)APDS9960_I2C_ADDR, (uint8_t *)&reg, 1, true);
    if (error != 0)
    {
        return false;
    }
    error = i2c->read((uint16_t)APDS9960_I2C_ADDR, (uint8_t *)&val, 1);
    if (error != 0)
    {
        return false;
    }
    return true;
}

static int wireReadDataBlock(uint8_t reg, uint8_t *val, unsigned int len)
{
    auto sda = LOOKUP_PIN(SDA);
    auto scl = LOOKUP_PIN(SCL);
    codal::I2C *i2c = pxt::getI2C(sda, scl);
    unsigned char i = 0;
    int error;
    error = i2c->write((uint16_t)APDS9960_I2C_ADDR, (uint8_t *)&reg, 1, true);
    if (error != 0)
    {
        return false;
    }
    error = i2c->read((uint16_t)APDS9960_I2C_ADDR, (uint8_t *)val, len);
    if (error != 0)
    {
        return false;
    }
    return len;
}

static void delay(uint16_t time)
{
    //uBit.sleep(time);
}

namespace SL06
{
static xSL06 *sl06ptr = new xSL06;

//%
void begin()
{
    sl06ptr->begin();
}

//%
uint8_t getMode()
{
    return sl06ptr->getMode();
}
//%
bool setMode(uint8_t mode, uint8_t enable)
{
    return sl06ptr->setMode(mode, enable);
}
//%
bool enablePower()
{
    return sl06ptr->enablePower();
}
//%
bool disablePower()
{
    return sl06ptr->disablePower();
}

//%
void enableGestureSensor(bool interrupts = false)
{
    sl06ptr->enableGestureSensor(interrupts);
}
//%
bool disableGestureSensor()
{
    return sl06ptr->disableGestureSensor();
}
//%
uint8_t getLEDDrive()
{
    return sl06ptr->getLEDDrive();
}
//%
bool setLEDDrive(uint8_t drive)
{
    return sl06ptr->setLEDDrive(drive);
}
//%
uint8_t getGestureLEDDrive()
{
    return sl06ptr->getGestureLEDDrive();
}
//%
bool setGestureLEDDrive(uint8_t drive)
{
    return sl06ptr->setGestureLEDDrive(drive);
}

uint8_t getGestureGain()
{
    return sl06ptr->getGestureGain();
}
//%
bool setGestureGain(uint8_t gain)
{
    return sl06ptr->setGestureGain(gain);
}

//%
uint8_t getGestureIntEnable()
{
    return sl06ptr->getGestureIntEnable();
}
//%
bool setGestureIntEnable(uint8_t enable)
{
    return sl06ptr->setGestureIntEnable(enable);
}

//%
bool isGestureAvailable()
{
    return sl06ptr->isGestureAvailable();
}
//%
int getGesture()
{
    return sl06ptr->getGesture();
}

//%
void enableProximitySensor(bool interrupts)
{
    sl06ptr->enableProximitySensor(interrupts);
}

//%
bool disableProximitySensor()
{
    return sl06ptr->disableProximitySensor();
}

//%
uint8_t getProximityGain()
{
    return sl06ptr->getProximityGain();
}

//%
void setProximityGain(uint8_t gain)
{
    sl06ptr->setProximityGain(gain);
}

//%
uint8_t getProximity()
{
    return sl06ptr->getProximity();
}

//%
void enableLightSensor(bool interrupts = false)
{
    sl06ptr->enableLightSensor(interrupts);
}

//%
void disableLightSensor()
{
    sl06ptr->disableLightSensor();
}

//%
uint8_t getAmbientLightGain(void)
{
    sl06ptr->getAmbientLightGain();
}

//%
void setAmbientLightGain(uint8_t gain)
{
    sl06ptr->setAmbientLightGain(gain);
}

//%
void clearAmbientLightInt()
{
    sl06ptr->clearAmbientLightInt();
}

//%
uint16_t getAmbientLight()
{
    return sl06ptr->getAmbientLight();
}

//%
uint16_t getRedLight()
{
    return sl06ptr->getRedLight();
}

//%
uint16_t getGreenLight()
{
    return sl06ptr->getGreenLight();
}

//%
uint16_t getBlueLight()
{
    return sl06ptr->getBlueLight();
}
} // namespace SL06
