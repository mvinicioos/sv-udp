# Servidor socket udp

Trabalho final da disciplina de sistemas operacionais 2.

### 📋 Requisitos de uso

#### Pré-requisitos

- Sistema operacional linux
- Compilador C++
- Conexão de rede

## 🚀 Iniciando

```bash
git clone https://github.com/mvinicioos/sv-udp
```

### Compilando

#### Servidor

```bash
g++ User.h User.cpp CLResponse.h GerUsers.cpp GerUsers.h CLResponse.cpp GerThreads.h GerThreads.cpp Server.h Server.cpp -o server -lpthread
```
#### Executando
```bash
./server
```

#### Client

```bash
g++ User.h User.cpp CLResponse.h CLResponse.cpp GerUsers.cpp GerUsers.h GerThreads.h GerThreads.cpp Client.h Client.cpp -o client -lpthread
```

#### Executando

```bash
./client nome ip_server 12345
```


