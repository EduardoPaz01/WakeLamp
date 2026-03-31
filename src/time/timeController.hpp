#ifndef TIMECONTROLLER_HPP
#define TIMECONTROLLER_HPP

#include <Arduino.h>
#include <time.h>
#include "timeClass.hpp"

/**
 * @class timeController
 * @brief Controlador central de tempo do sistema WakeLamp
 * @details Gerencia a hora do sistema, horários de despertar e intervalos de duração da lâmpada
 * @note A hora interna é ajustada para -3 horas (fuso horário de São Paulo)
 */
class timeController {

  private:
    /**
     * @brief Estrutura interna de tempo (tm struct)
     * @details Contém hora, minuto, segundo e outras informações de data/hora
     */
    struct tm TIME;

    /**
     * @brief Timestamp do último update de tempo (ms)
     * @details Usado para calcular tempo decorrido no método tick()
     */
    unsigned long lastUpdateMillis = 0;

    /**
     * @brief Horário padrão de despertar
     * @details Valor inicial: 5:30:00 (pode ser alterado via setWakeTime())
     */
    timeClass WAKE_TIME = timeClass(5, 30, 00); 

    /**
     * @brief Intervalo padrão de duração da lâmpada
     * @details Valor inicial: 1 hora (pode ser alterado via setLampInterval())
     */
    timeClass LAMP_INTERVAL = timeClass(1, 0, 00);

  public:
    /**
     * @brief Construtor da classe timeController
     * @details Inicializa o controlador de tempo com valores padrão (00:00:00)
     * @return Nenhum (construtor)
     * @note Deve ser chamado uma única vez na inicialização do sistema
     */
    timeController(void);

    /**
     * @brief Atualiza a hora interna com um novo valor de tempo
     * @details Sincroniza a hora do sistema com os dados recebidos do NTP ou outra fonte
     * @param current_time Estrutura tm contendo o novo tempo (hora, minuto, segundo)
     * @return Nenhum (void)
     * @note Aplica automaticamente ajuste de -3 horas (fuso horário São Paulo)
     * @example myTime.updateTime(wifiController.getTimeFromNTP());
     */
    void updateTime(struct tm current_time);

    /**
     * @brief Obtém a hora atual do sistema
     * @details Retorna a estrutura tm interna com os valores da hora atual
     * @return struct tm Hora, minuto e segundo atuais
     * @note A hora retornada já inclui o ajuste de fuso horário (-3h)
     * @example struct tm currentTime = myTime.getTime();
     */
    struct tm getTime(void); 

    /**
     * @brief Obtém o horário de despertar configurado
     * @details Retorna um objeto timeClass com a hora de despertar da lâmpada
     * @return timeClass Objeto contendo a hora, minuto e segundo de despertar
     * @note Valor padrão é 5:30:00
     * @example timeClass wakeTime = myTime.getWakeTime();
     */
    class timeClass getWakeTime(void);

    /**
     * @brief Obtém o intervalo de duração da lâmpada
     * @details Retorna a duração configurada que a lâmpada permanecerá acesa
     * @return timeClass Objeto contendo duração em horas, minutos e segundos
     * @note Valor padrão é 1:00:00 (1 hora)
     * @example timeClass interval = myTime.getLampInterval();
     */
    class timeClass getLampInterval(void);

    /**
     * @brief Define um novo horário de despertar
     * @details Atualiza o horário em que a lâmpada deve acender automaticamente
     * @param newTime Objeto timeClass com novo horário de despertar
     * @return Nenhum (void)
     * @note A mudança é imediata e afeta o próximo ciclo de despertar
     * @example myTime.setWakeTime(timeClass(6, 0, 0));
     */
    void setWakeTime(timeClass newTime);
    
    /**
     * @brief Define o intervalo de permanência da lâmpada acesa
     * @details Atualiza o tempo durante o qual a lâmpada permanecerá ligada após despertar
     * @param newTime Objeto timeClass com nova duração
     * @return Nenhum (void)
     * @note A mudança afeta o próximo ciclo de despertar
     * @example myTime.setLampInterval(timeClass(0, 30, 0));  // 30 minutos
     */
    void setLampInterval(timeClass newTime);

    /**
     * @brief Incrementa o tempo local baseado nos milissegundos decorridos
     * @details Realiza um tick no relógio interno atualizado pelo millis() do Arduino
     * @return Nenhum (void)
     * @note Deve ser chamado continuamente no loop() para manter tempo sincronizado
     * @note Gerencia automaticamente o overflow de segundos, minutos e horas
     * @example myTime.tick();  // Cada iteração do loop
     */
    void tick(void);
};

/**
 * @brief Função auxiliar para converter string de tempo em objeto timeClass
 * @details Recebe uma string no formato "HH:MM:SS" e retorna um objeto timeClass correspondente
 * @param data String contendo o horário no formato "HH:MM:SS"
 * @return timeClass Objeto contendo a hora, minuto e segundo parseados
 * @note A string deve estar no formato correto, caso contrário os valores podem ser inválidos
 */
timeClass parseTime(String data);

#endif // TIMECONTROLLER_HPP