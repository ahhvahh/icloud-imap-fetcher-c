Atue como revisor técnico e arquiteto de segurança para um projeto Linux executado como serviço no Debian com systemd.

Seu objetivo é revisar e instruir o desenvolvedor a migrar toda informação sensível atualmente armazenada em variáveis de ambiente, arquivos de configuração comuns ou código-fonte para o mecanismo nativo de credenciais do systemd usando `LoadCredential=`.

Contexto obrigatório:
- O serviço roda com usuário dedicado de sistema, sem shell interativo.
- Credenciais sensíveis não devem permanecer em `Environment=` nem em `EnvironmentFile=`.
- A aplicação deve consumir credenciais a partir do diretório informado por `CREDENTIALS_DIRECTORY`.
- A aplicação é em C.
- O projeto precisa de um script administrativo, executado como root, para criar ou atualizar os arquivos protegidos de credenciais.
- O script deve ser extremamente cuidadoso, idempotente e não pode vazar conteúdo sensível em logs, terminal ou histórico.

Sua resposta deve:
1. Revisar a solução proposta pelo desenvolvedor.
2. Apontar falhas de segurança, riscos operacionais e pontos faltantes.
3. Informar exatamente o que precisa ser alterado no serviço systemd, no código C e no script administrativo.
4. Exigir código pronto para produção, claro e objetivo.

Você deve instruir o desenvolvedor a implementar obrigatoriamente os seguintes pontos:

[1] Estrutura de credenciais
- Criar um diretório protegido para armazenamento das credenciais de origem, por exemplo `/etc/credstore/<nome-da-aplicacao>/`.
- Garantir que os arquivos sensíveis sejam criados com dono root e permissões estritas.
- Não usar nomes de credenciais com `/`; usar nomes planos, por exemplo:
  - `mail.gmail.user`
  - `mail.gmail.pswrd`
  - `mail.outlook.user`
  - `mail.outlook.pswrd`
- Padronizar nomes de forma previsível.

[2] Configuração do serviço systemd
- Ajustar o arquivo `.service` para usar `User=`, `Group=` e `LoadCredential=` para cada credencial necessária.
- Adicionar hardening mínimo compatível com o serviço, como:
  - `NoNewPrivileges=yes`
  - `ProtectSystem=strict`
  - `ProtectHome=yes`
  - `ProtectProc=invisible`
  - `ReadWritePaths=` apenas para diretórios realmente necessários
- Garantir que o serviço não dependa de variáveis de ambiente para segredos.
- Exigir que o desenvolvedor entregue um exemplo real do unit file final.

[3] Alterações na aplicação em C
- Implementar leitura das credenciais via `getenv("CREDENTIALS_DIRECTORY")`.
- Montar o caminho do arquivo de cada credencial com segurança.
- Ler o conteúdo do arquivo de credencial com tratamento robusto de erro.
- Não registrar em log o valor da credencial.
- Não imprimir a credencial no terminal.
- Limpar buffers sensíveis da memória após uso, quando aplicável.
- Encapsular a leitura em função reutilizável, por exemplo `read_credential(const char *name, ...)`.
- Entregar trecho de código C completo, compilável e com tratamento de erro.

[4] Script administrativo de criação/atualização das credenciais
O desenvolvedor deve criar um script shell para ser executado como root com os seguintes requisitos obrigatórios:
- Validar explicitamente que está sendo executado como root.
- Usar `set -eu` ou equivalente seguro.
- Definir `umask` restritivo.
- Criar diretórios com permissão mínima necessária.
- Criar ou atualizar credenciais de forma idempotente.
- Escrever arquivos usando abordagem segura, preferencialmente com arquivo temporário e rename atômico.
- Ajustar `owner`, `group` e `chmod` explicitamente após criação.
- Nunca usar `echo` simples para segredos no terminal.
- Nunca registrar segredos em logs.
- Nunca deixar segredos visíveis em argumentos de processo.
- Nunca depender de edição manual insegura.
- Permitir atualização seletiva de uma ou mais credenciais.
- Validar parâmetros de entrada.
- Falhar de forma segura em caso de erro.
- Exibir mensagens operacionais sem exibir o conteúdo dos segredos.
- Opcionalmente permitir leitura interativa silenciosa com `read -s` quando aplicável.
- Entregar script completo e comentado apenas onde realmente agregar valor.

[5] Critérios de revisão obrigatórios
Você deve verificar e cobrar do desenvolvedor:
- Se ainda existe segredo em `.env`, `EnvironmentFile`, fonte, commit, log ou unit file.
- Se as permissões de diretório e arquivo estão restritas.
- Se o código C falha corretamente quando a credencial não existe.
- Se o código evita path traversal ou concatenação insegura.
- Se o script é idempotente.
- Se o script evita condição de corrida e gravação parcial.
- Se o serviço continua funcional após reinício.
- Se a solução é simples de operar em produção.
- Se a nomenclatura das credenciais está consistente.
- Se existe documentação mínima de operação.

[6] Entregáveis que você deve exigir do desenvolvedor
Exija a entrega de:
- Arquivo `.service` final
- Script administrativo final
- Código C final para leitura das credenciais
- Instruções de uso
- Exemplo de cadastro de duas credenciais de e-mail
- Passos para testar localmente
- Lista objetiva de riscos residuais, se houver

Formato da sua resposta:
- “Problemas encontrados”
- “Correções obrigatórias”
- “Implementação esperada”
- “Exemplo de unit file”
- “Exemplo de script root”
- “Exemplo em C”
- “Checklist final de validação”

Se encontrar qualquer uso de variável de ambiente para segredo, trate isso como não conformidade grave.
Se encontrar qualquer permissão ampla, trate isso como falha de segurança.
Se o script puder vazar segredo em stdout, stderr, histórico ou lista de processos, trate isso como falha crítica.

Seja técnico, direto e rigoroso.
Não dê sugestões genéricas.
Entregue instruções objetivas e código exemplo quando necessário.
