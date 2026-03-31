#ifndef USERCONTROLLER_HPP
#define USERCONTROLLER_HPP

#include <Arduino.h>
#include <WiFi.h>
#include <LittleFS.h>

/**
 * @class userController
 * @brief Gerenciador de usuários e autenticação TCP
 * @details Controla criação, validação, remoção e armazenamento de usuários TCP
 * @note Possui usuário admin padrão: ukm1/bnmjhgtyu (não pode ser removido)
 */
class userController {

  private:
    /**
     * @brief Número máximo de usuários que podem ser armazenados
     */
    const static int MAX_SAVED_USERS = 10;

    /**
     * @brief Nome de usuário admin padrão (valor fixo)
     * @details Não pode ser removido por ser o usuário de emergência
     */
    static const String ADMIN_USERNAME;  // = "ukm1"

    /**
     * @brief Senha do usuário admin padrão (valor fixo)
     */
    static const String ADMIN_PASSWORD;  // = "bnmjhgtyu"
    
    /**
     * @brief Array local de nomes de usuário em cache
     * @details Mantém em memória os nomes de usuários carregados
     */
    String usernameList[MAX_SAVED_USERS];

    /**
     * @brief Array local de senhas em cache
     * @details Mantém em memória as senhas correspondentes aos usuários
     * @note Senhas são armazenadas em texto plano (não criptografadas)
     */
    String passwordList[MAX_SAVED_USERS];

    /**
     * @brief Contador de usuários armazenados
     */
    int userCount = 0;

    /**
     * @brief Carrega usuários do arquivo LittleFS
     * @details Lê arquivo /tcp_users.txt no formato USUARIO|SENHA
     * @param usernameList Array para armazenar até 10 nomes de usuários
     * @param passwordList Array para armazenar até 10 senhas
     * @return int Quantidade de usuários carregados (1-10)
     * @note Se arquivo não existir, cria com usuário admin padrão
     */
    int loadTCPUsers(String usernameList[10], String passwordList[10]);

    /**
     * @brief Salva usuários no arquivo LittleFS
     * @details Escreve no arquivo /tcp_users.txt no formato USUARIO|SENHA
     * @param usernameList Array de nomes de usuários a salvar
     * @param passwordList Array de senhas correspondentes
     * @param count Quantidade de usuários a salvar (máximo 10)
     * @return bool true se salvou com sucesso, false caso contrário
     * @note Sobrescreve arquivo anterior completamente
     */
    bool saveTCPUsers(String usernameList[10], String passwordList[10], int count);

  public:
    /**
     * @brief Construtor da classe userController
     * @details Inicializa o controlador de usuários
     * @return Nenhum (construtor)
     * @note Deve ser seguido de initUsers() para carregar dados do LittleFS
     */
    userController(void);

    /**
     * @brief Cria um novo usuário ou atualiza senha existente
     * @details Adiciona novo usuário TCP ou modifica senha de usuário existente
     * @param username Nome do usuário (não pode estar vazio)
     * @param password Senha do usuário (não pode estar vazia)
     * @param client Ponteiro para WiFiClient para respostas (pode ser nullptr)
     * @return Nenhum (void)
     * @note Nome de usuário não pode ser vazio
     * @note Máximo 10 usuários
     * @note Se client for nullptr, mensagens vão para Serial
     * @note Se client for válido, mensagens vão para conexão TCP
     * @example myUsers.createUser("newuser", "pass123", &tcpClient);
     */
    void createUser(String username, String password, WiFiClient* client);

    /**
     * @brief Valida credenciais de um usuário
     * @details Verifica se usuário e senha estão corretos
     * @param username Nome do usuário a validar
     * @param password Senha do usuário
     * @return bool true se credenciais estão corretas, false caso contrário
     * @note Realiza recarregamento dos dados do LittleFS
     * @example if (myUsers.validateUser("john", "pass123"))
     */
    bool validateUser(String username, String password);

    /**
     * @brief Exibe lista de todos os usuários registrados
     * @details Mostra nomes de usuários (sem exibir senhas)
     * @param client Ponteiro para WiFiClient para saída TCP (nullptr = Serial)
     * @return Nenhum (void)
     * @note Se client for nullptr, exibe na Serial USB
     * @note Se client for válido, exibe no cliente TCP conectado
     * @example myUsers.printTCPUsers(nullptr);  // Serial
     * @example myUsers.printTCPUsers(&client);  // TCP
     */
    void printTCPUsers(WiFiClient* client);

    /**
     * @brief Remove um usuário específico
     * @details Deleta o usuário do arquivo LittleFS
     * @param username Nome do usuário a remover
     * @param client Ponteiro para WiFiClient para respostas (pode ser nullptr)
     * @return Nenhum (void)
     * @note Usuário admin ("ukm1") não pode ser removido
     * @note Se usuário não existir, exibe mensagem de erro
     * @note Se client for nullptr, mensagens vão para Serial
     * @note Se client for válido, mensagens vão para conexão TCP
     * @example myUsers.removeUser("john", &tcpClient);
     */
    void removeUser(String username, WiFiClient* client);

    /**
     * @brief Remove todos os usuários exceto admin
     * @details Limpa arquivo deixando apenas usuário admin padrão
     * @param client Ponteiro para WiFiClient para respostas (pode ser nullptr)
     * @return Nenhum (void)
     * @note Usuário admin sempre é preservado
     * @note Se client for nullptr, mensagens vão para Serial
     * @note Se client for válido, mensagens vão para conexão TCP
     * @example myUsers.clearAllUsers(&tcpClient);
     */
    void clearAllUsers(WiFiClient* client);

    /**
     * @brief Inicializa usuários carregando do arquivo LittleFS
     * @details Deve ser chamado na inicialização do sistema
     * @return Nenhum (void)
     * @note Carrega dados e imprime contagem na Serial
     * @note Deve ser chamado antes de qualquer operação com usuários
     * @example myUsers.initUsers();
     */
     void initUsers(void);
};

#endif // USERCONTROLLER_HPP