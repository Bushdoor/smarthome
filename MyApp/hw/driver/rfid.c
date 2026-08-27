#include "rfid.h"
#include "spi.h"
#include <string.h>

extern SPI_HandleTypeDef hspi3;


// =================================
// RC522 Register
// =================================

#define CommandReg             0x01
#define ComIrqReg              0x04
#define ErrorReg               0x06
#define FIFODataReg            0x09
#define FIFOLevelReg           0x0A
#define ControlReg             0x0C
#define BitFramingReg          0x0D
#define ModeReg                0x11
#define TxControlReg           0x14
#define TxASKReg               0x15

#define TModeReg               0x2A
#define TPrescalerReg          0x2B
#define TReloadRegH            0x2C
#define TReloadRegL            0x2D


// =================================
// RC522 Command
// =================================

#define PCD_IDLE                0x00
#define PCD_TRANSCEIVE           0x0C

#define PICC_CMD_REQA            0x26
#define PICC_CMD_ANTICOLL        0x93


// =================================
// RFID 상태 변수
// =================================

uint8_t rfid_uid[RFID_MAX_UID_LEN] = {0};
uint8_t rfid_uid_size = 0;
uint8_t rfid_card_detected = 0;


// =================================
// 등록된 카드 목록
// =================================

static uint8_t registered_uids[RFID_MAX_CARDS][RFID_MAX_UID_LEN];
static uint8_t registered_count = 0;

static uint8_t last_uid[RFID_MAX_UID_LEN] = {0};
static uint8_t last_present = 0;


// =================================
// 함수 선언
// =================================

static void RFID_CS_Low(void);
static void RFID_CS_High(void);

static void RFID_WriteRegister(uint8_t reg, uint8_t value);
static uint8_t RFID_ReadRegister(uint8_t reg);

static void RFID_Reset(void);
static void RFID_AntennaOn(void);

static uint8_t RFID_Transceive(uint8_t *send_data,
                               uint8_t send_len,
                               uint8_t *recv_data,
                               uint8_t *recv_bits);


// =================================
// CS Low / High (핀 직접 입력)
// =================================

static void RFID_CS_Low(void)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_RESET);
}

static void RFID_CS_High(void)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_SET);
}


// =================================
// Register Write / Read
// =================================

static void RFID_WriteRegister(uint8_t reg, uint8_t value)
{
    uint8_t data[2];

    data[0] = (reg << 1) & 0x7E;
    data[1] = value;

    RFID_CS_Low();
    HAL_SPI_Transmit(&hspi3, data, 2, 100);
    RFID_CS_High();
}

static uint8_t RFID_ReadRegister(uint8_t reg)
{
    uint8_t tx[2];
    uint8_t rx[2] = {0};

    tx[0] = ((reg << 1) & 0x7E) | 0x80;
    tx[1] = 0x00;

    RFID_CS_Low();
    HAL_SPI_TransmitReceive(&hspi3, tx, rx, 2, 100);
    RFID_CS_High();

    return rx[1];
}


// =================================
// Reset (핀 직접 입력)
// =================================

static void RFID_Reset(void)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
    HAL_Delay(10);

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
    HAL_Delay(50);
}


// =================================
// Antenna ON
// =================================

static void RFID_AntennaOn(void)
{
    uint8_t value;

    value = RFID_ReadRegister(TxControlReg);

    if ((value & 0x03) != 0x03)
    {
        RFID_WriteRegister(TxControlReg, value | 0x03);
    }
}


// =================================
// Init (핀 직접 입력)
// =================================

void RFID_Init(void)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_SET);   // CS HIGH
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);    // RST HIGH

    HAL_Delay(10);

    RFID_Reset();

    // Timer
    RFID_WriteRegister(TModeReg, 0x8D);
    RFID_WriteRegister(TPrescalerReg, 0x3E);
    RFID_WriteRegister(TReloadRegL, 30);
    RFID_WriteRegister(TReloadRegH, 0);

    // 통신 모드
    RFID_WriteRegister(TxASKReg, 0x40);
    RFID_WriteRegister(ModeReg, 0x3D);

    // 안테나 ON
    RFID_AntennaOn();

    rfid_uid_size = 0;
    rfid_card_detected = 0;
}


// =================================
// Transceive
// =================================

static uint8_t RFID_Transceive(uint8_t *send_data,
                               uint8_t send_len,
                               uint8_t *recv_data,
                               uint8_t *recv_bits)
{
    uint8_t irq = 0;
    uint8_t error;
    uint8_t fifo_level;
    uint8_t last_bits;
    uint8_t i;
    uint8_t bit_framing;

    *recv_bits = 0;

    // FIFO Flush
    RFID_WriteRegister(FIFOLevelReg, 0x80);

    // Idle
    RFID_WriteRegister(CommandReg, PCD_IDLE);

    // IRQ Clear
    RFID_WriteRegister(ComIrqReg, 0x7F);

    // FIFO에 전송 데이터 저장
    for (i = 0; i < send_len; i++)
    {
        RFID_WriteRegister(FIFODataReg, send_data[i]);
    }

    // Transceive 시작
    RFID_WriteRegister(CommandReg, PCD_TRANSCEIVE);

    // StartSend (기존 TxLastBits 보존, StartSend 비트만 추가)
    bit_framing = RFID_ReadRegister(BitFramingReg);
    RFID_WriteRegister(BitFramingReg, bit_framing | 0x80);

    // 응답 대기
    for (i = 0; i < 100; i++)
    {
        irq = RFID_ReadRegister(ComIrqReg);

        if (irq & 0x30)
        {
            break;
        }

        HAL_Delay(1);
    }

    // StartSend 해제 (TxLastBits 유지)
    RFID_WriteRegister(BitFramingReg, bit_framing);

    // Timeout
    if (!(irq & 0x30))
    {
        return 0;
    }

    // Error 확인
    error = RFID_ReadRegister(ErrorReg);

    if (error & 0x1B)
    {
        return 0;
    }

    // FIFO 데이터 개수
    fifo_level = RFID_ReadRegister(FIFOLevelReg);

    if (fifo_level > 10)
    {
        fifo_level = 10;
    }

    // 마지막 비트
    last_bits = RFID_ReadRegister(ControlReg) & 0x07;

    if (last_bits)
    {
        *recv_bits = (fifo_level - 1) * 8 + last_bits;
    }
    else
    {
        *recv_bits = fifo_level * 8;
    }

    // FIFO 읽기
    for (i = 0; i < fifo_level; i++)
    {
        recv_data[i] = RFID_ReadRegister(FIFODataReg);
    }

    return 1;
}


// =================================
// UID Read
// =================================

uint8_t RFID_ReadUID(void)
{
    uint8_t command;
    uint8_t anticoll_cmd[2];
    uint8_t response[10];
    uint8_t response_bits;
    uint8_t i;

    rfid_card_detected = 0;
    rfid_uid_size = 0;

    // -----------------------------
    // REQA (7비트 짧은 프레임)
    // -----------------------------

    command = PICC_CMD_REQA;

    RFID_WriteRegister(BitFramingReg, 0x07);

    if (!RFID_Transceive(&command, 1, response, &response_bits))
    {
        return 0;
    }

    // ATQA = 16bit
    if (response_bits != 16)
    {
        return 0;
    }

    // -----------------------------
    // Anti Collision (0x93 + NVB(0x20))
    // -----------------------------

    anticoll_cmd[0] = PICC_CMD_ANTICOLL;
    anticoll_cmd[1] = 0x20;

    RFID_WriteRegister(BitFramingReg, 0x00);

    if (!RFID_Transceive(anticoll_cmd, 2, response, &response_bits))
    {
        return 0;
    }

    // UID 4byte + BCC
    if (response_bits < 40)
    {
        return 0;
    }

    // UID 저장
    for (i = 0; i < RFID_MAX_UID_LEN; i++)
    {
        rfid_uid[i] = response[i];
    }

    rfid_uid_size = RFID_MAX_UID_LEN;
    rfid_card_detected = 1;

    return 1;
}


// =================================
// 카드 등록
// =================================

void RFID_RegisterCard(const uint8_t *uid)
{
    if (registered_count >= RFID_MAX_CARDS)
    {
        return;
    }

    memcpy(registered_uids[registered_count], uid, RFID_MAX_UID_LEN);
    registered_count++;
}


// =================================
// 등록 여부 확인
// =================================

uint8_t RFID_IsRegisteredUID(const uint8_t *uid)
{
    uint8_t i;

    for (i = 0; i < registered_count; i++)
    {
        if (memcmp(registered_uids[i], uid, RFID_MAX_UID_LEN) == 0)
        {
            return 1;
        }
    }

    return 0;
}


// =================================
// 인증 (등록된 카드인지까지 확인)
// =================================

uint8_t RFID_IsAuthorized(void)
{
    if (!RFID_ReadUID())
    {
        return 0;
    }

    return RFID_IsRegisteredUID(rfid_uid);
}


// =================================
// 새로 태그된 카드만 감지 (디바운싱 + 엣지 검출)
// =================================

uint8_t RFID_CheckAndGetEdge(uint8_t *out_uid)
{
    uint8_t detected;
    uint8_t is_new_card;

    detected = RFID_ReadUID();

    if (!detected)
    {
        last_present = 0;
        return 0;
    }

    is_new_card = (last_present == 0) ||
                  (memcmp(last_uid, rfid_uid, RFID_MAX_UID_LEN) != 0);

    memcpy(last_uid, rfid_uid, RFID_MAX_UID_LEN);
    last_present = 1;

    if (!is_new_card)
    {
        return 0;
    }

    if (out_uid != NULL)
    {
        memcpy(out_uid, rfid_uid, RFID_MAX_UID_LEN);
    }

    return 1;
}