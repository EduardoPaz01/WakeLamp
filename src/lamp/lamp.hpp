#ifndef LAMP_HPP
#define LAMP_HPP

#include <Arduino.h>

#define BUTTON_PIN 34
#define OUTPUT_PIN 25
#define LED_PIN 2

/**
 * @class lamp
 * @brief Controlador da lâmpada do sistema WakeLamp
 * @details Gerencia o estado da lâmpada e controla o envio de pulsos para ativação
 * @note Usa um sistema de pulso de 10ms para ativar a lâmpada
 */
class lamp {

  private:
    /**
     * @brief Número do pino de controle da lâmpada
     * @note Configurado através da constante global OUTPUT_PIN
     */
    int LAMP = 0;

    /**
     * @brief Estado atual da lâmpada
     * @details true = lâmpada acesa, false = lâmpada apagada
     */
    bool lampCurrentState = false;

    /**
     * @brief Estado desejado para a lâmpada
     * @details Quando true, será enviado um pulso na próxima chamada de updatedLamp()
     */
    bool lampDesiredState = false;

    /**
     * @brief Timestamp do último toggle enviado (ms)
     * @details Controla o débounce entre pulsos
     */
    unsigned long lastToggleTime = 0;

    /**
     * @brief Atraso mínimo entre pulsos (ms)
     * @details Evita envio de múltiplos pulsos muito próximos
     */
    const unsigned long TOGGLE_DELAY = 50;  ///< 50 milissegundos

  public:

    /**
     * @brief Armazena o estado anterior da condição de tempo
     * @details Detecta transição entre período de ON/OFF da lâmpada
     * @note Propriedade pública para leitura pelo sistema (para debounce de tempo)
     */
    bool lastTimeState = false;

    /**
     * @brief Construtor da classe lamp
     * @details Inicializa o controlador de lâmpada
     * @return Nenhum (construtor)
     * @note Estado inicial é apagado (false)
     */
    lamp();

    /**
     * @brief Ativa a lâmpada
     * @details Envia um comando para acender a lâmpada (único pulso)
     * @return Nenhum (void)
     * @note Se já estiver acesa, não realiza nova ativação
     * @note A ativação é agendada para o próximo updatedLamp()
     * @example myLamp.lampOn();
     */
    void lampOn();

    /**
     * @brief Desativa a lâmpada
     * @details Envia um comando para apagar a lâmpada (único pulso)
     * @return Nenhum (void)
     * @note Se já estiver apagada, não realiza nova desativação
     * @note A desativação é agendada para o próximo updatedLamp()
     * @example myLamp.lampOff();
     */
    void lampOff();

    /**
     * @brief Programa um novo pulso de toggle
     * @details Agenda o envio de um pulso para alternar o estado
     * @return Nenhum (void)
     * @note O pulso efetivo é enviado em updatedLamp()
     * @note Método interno - não chamado via comando remoto
     */
    void toggleLamp();

    /**
     * @brief Processa e envia pulsos agendados
     * @details Executa o envio do pulso se foi agendado e suficiente tempo passou
     * @return Nenhum (void)
     * @note Deve ser chamado continuamente no loop() para funcionamento correto
     * @note Respeita TOGGLE_DELAY entre pulsos (50ms)
     * @note Pulso dura 10ms com HIGH seguido de LOW
     * @example myLamp.updatedLamp();  // Em cada iteração do loop
     */
    void updatedLamp();
};

#endif // LAMP_HPP