/**
 */
class lamp {
  private:
    /**
     * Pin number for lamp 1
     */
    int lamp_1 = 0;

    /**
     * Pin number for lamp 2
     */
    int lamp_2 = 0;

    /**
     * Pin number for button
     */
    int button = 0;

  public:

    /**
     * Constructor of the lamp class
     */
    lamp(uint8_t lamp_1_pin = 12, uint8_t lamp_2_pin = 13, uint8_t button_pin = 14);

    /**
     * Turn on lamp 1
     */
    void lamp_1_on();

    /**
     * Turn off lamp 1
     */
    void lamp_1_off();

    /**
     * Turn on lamp 2
     */
    void lamp_2_on();

    /**
     * Turn off lamp 2
     */
    void lamp_2_off();
};