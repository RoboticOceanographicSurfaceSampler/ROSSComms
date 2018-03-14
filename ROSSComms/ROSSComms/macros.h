

/*** USER CLASS MACROS ***/

#define STATUS_SET(void) (PORTC.OUTSET = PIN2_bm)
#define STATUS_CLR(void) (PORTC.OUTCLR = PIN2_bm)
#define STATUS_TOGGLE(void) ((PORTC.OUT & PIN2_bm) == 0 ? STATUS_SET() : STATUS_CLR())

#define ERROR_SET(void) (PORTC.OUTSET = PIN3_bm)
#define ERROR_CLR(void) (PORTC.OUTCLR = PIN3_bm)
#define ERROR_TOGGLE(void) ((PORTC.OUT & PIN3_bm) == 0 ? ERROR_SET() : ERROR_CLR())


//Switch Input Macros
#define CHECK_TX_SW(void) (!(PORTA.IN & PIN4_bm)) //Returns true if the radios should TX


//RSSI Input from the XTend
#define READ_RSSI_PIN(void) (PORTA.IN & PIN2_bm)

/*** "SYSTEM" Class MACROS ***/

//Interrupt enables
#define LOW_LEVEL_INTERRUPTS_ENABLE()  (PMIC.CTRL |= PMIC_LOLVLEN_bm)
#define MED_LEVEL_INTERRUPTS_ENABLE()  (PMIC.CTRL |= PMIC_MEDLVLEN_bm)
#define HIGH_LEVEL_INTERRUPTS_ENABLE() (PMIC.CTRL |= PMIC_HILVLEN_bm)