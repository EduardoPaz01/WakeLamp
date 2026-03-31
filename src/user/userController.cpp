#include "userController.hpp"

userController::userController(void) {
  Serial.println("[USERS] LittleFS (2MB SPIFFS) ativado para salvar usuarios TCP");
}

void userController::initUsers(void) {
  userCount = loadTCPUsers(usernameList, passwordList);
  Serial.print("[USERS] Total de usuarios carregados: ");
  Serial.println(userCount);
}

int userController::loadTCPUsers(String usernameList[10], String passwordList[10]) {
  int count = 0;
  File file = LittleFS.open("/tcp_users.txt", "r");
  if (!file) {
    Serial.println("[USERS] Arquivo /tcp_users.txt nao encontrado. Criando com admin padrao...");
    // Criar arquivo com admin padrão
    File newFile = LittleFS.open("/tcp_users.txt", "w");
    if (newFile) {
      newFile.print(ADMIN_USERNAME);
      newFile.print('|');
      newFile.println(ADMIN_PASSWORD);
      newFile.close();
    }
    usernameList[0] = ADMIN_USERNAME;
    passwordList[0] = ADMIN_PASSWORD;
    return 1;
  }

  while (file.available() && count < 10) {
    String line = file.readStringUntil('\n');
    
    if (line.endsWith("\r")) {
      line = line.substring(0, line.length() - 1);
    }
    
    line.trim();
    
    if (line.length() == 0) {
      continue;
    }

    int sep = line.indexOf('|');
    if (sep == -1) {
      Serial.print("[USERS] Linha sem separador |: ");
      Serial.println(line);
      continue;
    }

    usernameList[count] = line.substring(0, sep);
    passwordList[count] = line.substring(sep + 1);
    
    Serial.print("[USERS] [");
    Serial.print(count);
    Serial.print("] Usuario: '");
    Serial.print(usernameList[count]);
    Serial.println("'");
    
    count++;
  }
  
  file.close();
  return count;
}

bool userController::saveTCPUsers(String usernameList[10], String passwordList[10], int count) {
  File file = LittleFS.open("/tcp_users.txt", "w");
  if (!file) {
    Serial.println("[USERS] ERRO: Nao conseguiu abrir arquivo para escrita");
    return false;
  }

  for (int i = 0; i < count; i++) {
    file.print(usernameList[i]);
    file.print('|');
    file.println(passwordList[i]);
  }
  
  file.flush();
  file.close();
  
  File checkFile = LittleFS.open("/tcp_users.txt", "r");
  if (checkFile) {
    Serial.print("[USERS] Arquivo salvo com sucesso. Tamanho: ");
    Serial.print(checkFile.size());
    Serial.println(" bytes");
    checkFile.close();
  }
  
  return true;
}

void userController::createUser(String username, String password, WiFiClient* client) {
  // Validar entrada
  if (username.length() == 0 || password.length() == 0) {
    if (client) client->println("\n[ERRO] Usuario e senha nao podem estar vazios!");
    else Serial.println("[ERRO] Usuario e senha nao podem estar vazios!");
    return;
  }

  // Carregar usuários atuais
  String tempUsernames[10];
  String tempPasswords[10];
  userCount = loadTCPUsers(tempUsernames, tempPasswords);

  // Verificar se já existe
  bool exists = false;
  for (int i = 0; i < userCount; i++) {
    if (tempUsernames[i] == username) {
      // Atualizar se já existe
      tempPasswords[i] = password;
      exists = true;
      break;
    }
  }

  // Se não existe, adicionar novo
  if (!exists) {
    if (userCount >= 10) {
      if (client) client->println("\n[ERRO] Limite de 10 usuarios atingido!");
      else Serial.println("[ERRO] Limite de 10 usuarios atingido!");
      return;
    }
    tempUsernames[userCount] = username;
    tempPasswords[userCount] = password;
    userCount++;
  }

  // Salvar no LittleFS
  if (saveTCPUsers(tempUsernames, tempPasswords, userCount)) {
    // Recarregar
    userCount = loadTCPUsers(usernameList, passwordList);
    
    if (client) {
      client->print("\n[USERS] Usuario '");
      client->print(username);
      client->println(exists ? "' atualizado com sucesso!" : "' criado com sucesso!");
    } else {
      Serial.print("[USERS] Usuario '");
      Serial.print(username);
      Serial.println(exists ? "' atualizado com sucesso!" : "' criado com sucesso!");
    }
  } else {
    if (client) client->println("\n[ERRO] Falha ao gravar usuarios no LittleFS!");
    else Serial.println("[ERRO] Falha ao gravar usuarios no LittleFS!");
  }
}

bool userController::validateUser(String username, String password) {
  // Sempre recarregar para ter dados atualizados
  userCount = loadTCPUsers(usernameList, passwordList);
  
  for (int i = 0; i < userCount; i++) {
    if (usernameList[i] == username && passwordList[i] == password) {
      return true;
    }
  }
  return false;
}

void userController::printTCPUsers(WiFiClient* client) {
  // Recarregar dados
  userCount = loadTCPUsers(usernameList, passwordList);

  if (client == nullptr) {  // Serial
    Serial.println("\n=== USUARIOS TCP REGISTRADOS ===");
    Serial.print("Total: ");
    Serial.println(userCount);
    
    if (userCount == 0) {
      Serial.println("Nenhum usuario registrado.");
      return;
    }
    
    for (int i = 0; i < userCount; i++) {
      Serial.print("  [");
      Serial.print(i);
      Serial.print("] ");
      Serial.println(usernameList[i]);
    }
  } else {  // TCP
    client->println("\n=== USUARIOS TCP REGISTRADOS ===");
    client->print("Total: ");
    client->println(userCount);
    
    if (userCount == 0) {
      client->println("Nenhum usuario registrado.");
      return;
    }
    
    for (int i = 0; i < userCount; i++) {
      client->print("  [");
      client->print(i);
      client->print("] ");
      client->println(usernameList[i]);
    }
  }
}

void userController::removeUser(String username, WiFiClient* client) {
  // Não permitir remover admin
  if (username == ADMIN_USERNAME) {
    if (client) client->println("\n[ERRO] O usuario admin (ukm1) nao pode ser removido!");
    else Serial.println("[ERRO] O usuario admin (ukm1) nao pode ser removido!");
    return;
  }

  // Recarregar usuários
  String tempUsernames[10];
  String tempPasswords[10];
  userCount = loadTCPUsers(tempUsernames, tempPasswords);

  // Procurar e remover
  bool found = false;
  for (int i = 0; i < userCount; i++) {
    if (tempUsernames[i] == username) {
      found = true;
      // Remover deslocando os seguintes
      for (int j = i; j < userCount - 1; j++) {
        tempUsernames[j] = tempUsernames[j + 1];
        tempPasswords[j] = tempPasswords[j + 1];
      }
      userCount--;
      break;
    }
  }

  if (!found) {
    if (client) {
      client->print("\n[ERRO] Usuario '");
      client->print(username);
      client->println("' nao encontrado!");
    } else {
      Serial.print("[ERRO] Usuario '");
      Serial.print(username);
      Serial.println("' nao encontrado!");
    }
    return;
  }

  // Salvar no LittleFS
  if (saveTCPUsers(tempUsernames, tempPasswords, userCount)) {
    // Recarregar
    userCount = loadTCPUsers(usernameList, passwordList);
    
    if (client) {
      client->print("\n[USERS] Usuario '");
      client->print(username);
      client->println("' removido com sucesso!");
    } else {
      Serial.print("[USERS] Usuario '");
      Serial.print(username);
      Serial.println("' removido com sucesso!");
    }
  } else {
    if (client) client->println("\n[ERRO] Falha ao gravar usuarios no LittleFS!");
    else Serial.println("[ERRO] Falha ao gravar usuarios no LittleFS!");
  }
}

void userController::clearAllUsers(WiFiClient* client) {
  // Criar array apenas com admin
  String tempUsernames[10];
  String tempPasswords[10];
  tempUsernames[0] = ADMIN_USERNAME;
  tempPasswords[0] = ADMIN_PASSWORD;
  int count = 1;

  // Salvar no LittleFS
  if (saveTCPUsers(tempUsernames, tempPasswords, count)) {
    userCount = 1;
    
    if (client) {
      client->println("\n[USERS] Todos os usuarios foram removidos (exceto admin)!");
    } else {
      Serial.println("[USERS] Todos os usuarios foram removidos (exceto admin)!");
    }
  } else {
    if (client) client->println("\n[ERRO] Falha ao gravar usuarios no LittleFS!");
    else Serial.println("[ERRO] Falha ao gravar usuarios no LittleFS!");
  }
}
