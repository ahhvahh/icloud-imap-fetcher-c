# Security Policy

## Supported Versions

Este projeto ainda está em evolução e a branch `main` é tratada como a única versão com correções de segurança.

## Reporting a Vulnerability

Para relatar vulnerabilidades, abra uma issue privada (se disponível) ou envie um relato para o mantenedor do repositório com:

1. Descrição do problema.
2. Impacto esperado.
3. Passos mínimos para reprodução.
4. Versão/commit afetado.
5. Mitigação sugerida (opcional).

Evite publicar credenciais, app-specific password, tokens ou exemplos com dados reais.

## Hardening Baseline

- Use app-specific password do iCloud em vez da senha principal.
- Mantenha o arquivo de configuração com permissão `0600`.
- Restrinja o diretório de logs para evitar exposição de metadados sensíveis.
- Revogue e regenere a app-specific password em caso de suspeita de vazamento.
