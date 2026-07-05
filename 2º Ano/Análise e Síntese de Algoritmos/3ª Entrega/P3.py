from pulp import *

# Leitura de input
n, m, t = map(int, input().split())

# Leitura e filtro de fábricas com stock positivo
fabricas_com_stock = {}
pais_fabrica = {}
for _ in range(n):
    fid, pais, stock_max = map(int, input().split())
    if stock_max > 0:
        fabricas_com_stock[fid] = stock_max
        pais_fabrica[fid] = pais

# Se não houver fábricas com stock, termina logo
if not fabricas_com_stock:
    print(-1)
    exit()

# Leitura dos países e requisitos mínimos
paises = {}
min_presentes = {}
for _ in range(m):
    pid, exportacoes_max, min_local = map(int, input().split())
    paises[pid] = exportacoes_max
    min_presentes[pid] = min_local

# Inicialização das estruturas de dados
criancas_por_pais = {p: [] for p in paises}
criancas = {}
crianca_pais = {}
pares_validos = []
fabricas_por_pais = {p: set() for p in paises}

# Pré-calculo das fabricas por país
for f, p in pais_fabrica.items():
    fabricas_por_pais[p].add(f)

# Leitura e processamento das crianças
for _ in range(t):
    data = list(map(int, input().split()))
    cid, pais = data[0], data[1]
    pedidos_validos = set(data[2:]) & fabricas_com_stock.keys()
    
    if pedidos_validos:
        criancas[cid] = pedidos_validos
        crianca_pais[cid] = pais
        criancas_por_pais[pais].append(cid)
        pares_validos.extend((cid, f) for f in pedidos_validos)

# Validação
for pais, min_req in min_presentes.items():
    if len(criancas_por_pais[pais]) < min_req:
        print(-1)
        exit()

# Criação do problema
prob = LpProblem("P3", LpMaximize)
x = LpVariable.dicts("p", pares_validos, cat='Binary')

# Função objetivo
prob += lpSum(x[c,f] for (c,f) in pares_validos)

# Dicionário para cache de restrições frequentes
pares_por_crianca = {}
pares_por_fabrica = {}
export_por_pais = {p: [] for p in paises}
for c, f in pares_validos:
    pares_por_crianca.setdefault(c, []).append(f)
    pares_por_fabrica.setdefault(f, []).append(c)
    pf = pais_fabrica[f]
    if crianca_pais[c] != pf:
        export_por_pais[pf].append((c,f))

#Restrições

# 1ª Restrição: Cada criança recebe no máximo 1 presente
for c, fabricas in pares_por_crianca.items():
    prob += lpSum(x[c,f] for f in fabricas) <= 1

# 2ª Restrição: As fabricas têm um stock máximo
for f, criancas_f in pares_por_fabrica.items():
    prob += lpSum(x[c,f] for c in criancas_f) <= fabricas_com_stock[f]

# 3ª Restrição: Cada país tem que receber um número minimo de presentes
for pais in paises:
    prob += lpSum(x[c,f] 
                 for c in criancas_por_pais[pais] 
                 for f in criancas[c]) >= min_presentes[pais]

# 4ª Restrição: Cada país tem um máximo de exportações
for pais, exportacoes in export_por_pais.items():
    prob += lpSum(x[c, f] for c, f in exportacoes) <= paises[pais]
    
#Resolução do problema
prob.solve(GLPK(msg=0))

#Output
print(-1 if prob.status == -1 else int(value(prob.objective)))