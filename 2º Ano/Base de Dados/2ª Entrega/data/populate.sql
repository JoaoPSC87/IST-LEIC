-- Popular tabela aeroporto
\echo 'Carregando dados da tabela aeroporto...'
\copy aeroporto(codigo, nome, cidade, pais) FROM '~/data/Aeroportos.txt' DELIMITER ',';

-- Popular tabela aviao
\echo 'Carregando dados da tabela aviao...'
\copy aviao(no_serie, modelo) FROM '~/data/Avioes.txt' DELIMITER ',' ;

-- Popular tabela assento
\echo 'Carregando dados da tabela assento...'
\copy assento(lugar, no_serie, prim_classe) FROM '~/data/Assentos.txt' DELIMITER ',' ;

-- Popular tabela voo
\echo 'Carregando dados da tabela voo...'
\copy voo(id, no_serie, hora_partida, hora_chegada, partida, chegada) FROM '~/data/Voos.txt' DELIMITER ',';

-- Popular tabela venda
\echo 'Carregando dados da tabela venda...'
\copy venda(codigo_reserva, nif_cliente, balcao, hora) FROM '~/data/Vendas.txt' DELIMITER ',' ;

-- Popular tabela bilhete
\echo 'Carregando dados da tabela bilhete...'
\copy bilhete(id, voo_id, codigo_reserva, nome_passegeiro, preco, prim_classe, lugar, no_serie) FROM '~/data/Bilhetes.txt' DELIMITER ',';

-- Atualizar sequências (para campos SERIAL)
SELECT setval('voo_id_seq', (SELECT MAX(id) FROM voo));
SELECT setval('venda_codigo_reserva_seq', (SELECT MAX(codigo_reserva) FROM venda));
SELECT setval('bilhete_id_seq', (SELECT MAX(id) FROM bilhete));

\echo 'Dados carregados com sucesso!'

-- Verificar contagem de registos em cada tabela
\echo 'Verificando contagem de registos:'
SELECT 'aeroporto' as tabela, COUNT(*) as registos FROM aeroporto
UNION ALL
SELECT 'aviao' as tabela, COUNT(*) as registos FROM aviao
UNION ALL
SELECT 'assento' as tabela, COUNT(*) as registos FROM assento
UNION ALL
SELECT 'voo' as tabela, COUNT(*) as registos FROM voo
UNION ALL
SELECT 'venda' as tabela, COUNT(*) as registos FROM venda
UNION ALL
SELECT 'bilhete' as tabela, COUNT(*) as registos FROM bilhete;