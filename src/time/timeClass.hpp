#ifndef TIMECLASS_HPP
#define TIMECLASS_HPP

/**
 * @class timeClass
 * @brief Classe simples para representar um ponto no tempo com horas, minutos e segundos
 * @details Armazena três valores inteiros que representam um horário completo
 */
class timeClass {
  public:
    unsigned int HOUR;      ///< Hora (0-23)
    unsigned int MINUTE;    ///< Minuto (0-59)
    unsigned int SECOND;    ///< Segundo (0-59)

    /**
     * @brief Construtor da classe timeClass
     * @details Inicializa os valores de hora, minuto e segundo
     * @param HOUR_ Valor da hora a ser armazenado
     * @param MINUTE_ Valor do minuto a ser armazenado
     * @param SECOND_ Valor do segundo a ser armazenado
     * @return Nenhum (construtor)
     * @note Os valores não são validados. Certifique-se de passar valores válidos
     * @example timeClass wakeTime(5, 30, 0);  // Cria um horário de 5:30:00
     */
    timeClass(const unsigned int HOUR_, const unsigned int MINUTE_, const unsigned int SECOND_) {
      HOUR = HOUR_;
      MINUTE = MINUTE_;
      SECOND = SECOND_;
    }
};


#endif // TIMECLASS_HPP