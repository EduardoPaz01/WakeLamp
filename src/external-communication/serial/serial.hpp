#ifndef SERIAL_HPP
#define SERIAL_HPP

#include <Arduino.h>
#include "serial-codes.hpp"

/**
 * @class serial
 * @brief Gerenciador de comunicação serial USB com o ESP32
 * @details Implementa leitura de comandos e escrita de respostas via porta serial
 * @note A frequência padrão é 115200 baud
 */
class serial {
  private:
    /**
     * @brief Frequência de comunicação serial em baud
     * @details Típico: 9600, 38400, 115200
     */
    int FREQUENCY;

    /**
     * @brief Buffer de cache para entrada de dados serial
     * @details Armazena a linha completa recebida até quebra de linha (\\n)
     */
    String CACHE_LINE;

    /**
     * @brief Buffer para parâmetros de comandos
     * @details Armazena os parâmetros extraídos de comandos para processamento posterior
     */
    String BUFFER;

  public:

    /**
     * @brief Construtor da classe serial
     * @details Inicializa a porta serial com a frequência especificada
     * @param FREQUENCY_SERIAL Frequência de comunicação em baud (padrão: 115200)
     * @return Nenhum (construtor)
     * @note Ativa a porta serial do Arduino/ESP32
     * @example serial mySerial(115200);
     */
    serial(int FREQUENCY_SERIAL = 115200);

    /**
     * @brief Destrutor da classe serial
     * @details Encerra a comunicação serial
     * @return Nenhum (destrutor)
     * @note Chama Serial.end() para liberar recursos
     */
    ~serial();

    /**
     * @brief Processa dados recebidos na porta serial
     * @details Lê uma linha completa e identifica qual comando foi enviado
     * @return uint32_t ID do comando identificado (ex: GET_TIME, SET_WAKE_TIME, etc)
     * @retval UNKNOWN_CMD Se nenhum comando válido for identificado
     * @note Comandos devem estar no formato: $COMANDO.PARAMETROS
     * @note Retorna UNKNOWN_CMD se nenhuma linha completa foi recebida
     * @example uint32_t cmd = mySerial.processSerial();
     */
    uint32_t processSerial();

    /**
     * @brief Imprime uma mensagem na porta serial
     * @details Escreve uma String na saída serial sem quebra de linha automática
     * @param MESSAGE String contendo a mensagem a ser impressa
     * @return Nenhum (void)
     * @note Não adiciona \\n automaticamente; use \\\\n na string para quebrar linhas
     * @example mySerial.print("HORA: " + String(hora));
     */
    void print(String MESSAGE);

    /**
     * @brief Obtém o buffer de parâmetros do último comando
     * @details Retorna os dados extraídos do último comando processado
     * @return String Conteúdo do buffer com parâmetros
     * @note Buffer é limpo após cada processamento
     * @example String params = mySerial.getBuffer();
     */
    String getBuffer();

};

#endif // SERIAL_HPP