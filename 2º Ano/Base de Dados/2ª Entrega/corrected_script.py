import random
import bisect
from datetime import datetime, timedelta
import csv
from io import StringIO

# Configurações
AEROPORTOS = ['LIS', 'OPO', 'CDG', 'ORY', 'FRA', 'MUC', 'MAD', 'BCN', 'AMS', 'BRU', 'FCO']
AVIOES = ['SN000', 'SN001', 'SN002', 'SN003', 'SN004', 'SN005', 'SN006', 'SN007', 'SN008', 'SN009']

PERNAS_POR_DIA = (2, 6)
NUM_VENDAS = 12000
BILHETES_POR_VOO = (4, 18)


NOMES_PASSAGEIROS = [
    'João Silva', 'Maria Santos', 'Pedro Costa', 'Ana Oliveira', 'Carlos Pereira',
    'Sofia Rodrigues', 'Miguel Ferreira', 'Catarina Martins', 'Bruno Almeida', 'Inês Carvalho',
    'Ricardo Sousa', 'Joana Ribeiro', 'António Gomes', 'Beatriz Lima', 'Paulo Dias',
    'Sara Fernandes', 'Nuno Tavares', 'Marta Pinto', 'Daniel Correia', 'Francisca Moreira',
    'Tiago Lopes', 'Vera Campos', 'Hugo Baptista', 'Lídia Cunha', 'Rui Neves',
    'Cristina Rocha', 'Filipe Barbosa', 'Patrícia Moura', 'André Cardoso', 'Sónia Vaz',
    'Helena Matos', 'Eduardo Figueiredo', 'Leonor Teixeira', 'Gustavo Pires', 'Matilde Antunes',
    'Diogo Loureiro', 'Raquel Simões', 'Alexandre Castro', 'Teresa Guedes', 'Luís Fonseca',
    'Cláudia Azevedo', 'Gabriel Mendes', 'Isabel Nunes', 'Rafael Coelho', 'Susana Leal',
    'Bruna Monteiro', 'Dinis Ramos', 'Mariana Barros', 'Tomás Henriques', 'Débora Salgado',
    'John Smith', 'Emily Johnson', 'Michael Brown', 'Jessica Williams', 'David Miller',
    'Sarah Davis', 'James Wilson', 'Ashley Moore', 'Robert Taylor', 'Amanda Anderson',
    'William Thomas', 'Linda Jackson', 'Christopher White', 'Barbara Harris', 'Matthew Martin',
    'Elizabeth Thompson', 'Joshua Garcia', 'Jennifer Martinez', 'Andrew Robinson', 'Patricia Clark',
    'Daniel Lewis', 'Mary Lee', 'Anthony Walker', 'Susan Hall', 'Mark Allen',
    'Karen Young', 'Paul King', 'Nancy Wright', 'Steven Scott', 'Donna Green',
    'Kevin Adams', 'Lisa Baker', 'Brian Nelson', 'Sandra Carter', 'Jason Mitchell',
    'Kimberly Perez', 'Eric Roberts', 'Donna Evans', 'Ryan Turner', 'Laura Phillips',
    'Jacob Campbell', 'Megan Parker', 'Benjamin Edwards', 'Emma Collins', 'Samuel Stewart',
    'Olivia Morris', 'Alexander Rogers', 'Sophia Cook', 'Ethan Reed', 'Chloe Morgan'
] # Gerado com recurso ao GPT-4.1

class GeradorDados:
    def __init__(self):
        self.voos = []; self.vendas = []; self.bilhetes = []
        self.contador_voo = 1; self.contador_venda = 1; self.contador_bilhete = 1
        self.assentos_ocupados = {}  # {voo_id: [lugares_ocupados]}
        self.data_hoje = datetime.now()
        self.carregar_assentos()

    def carregar_assentos(self):
        """Lê os assentos reais do Assentos.txt, agrupados por avião e classe"""
        self.assentos_por_aviao = {}
        with open('Assentos.txt', encoding='utf-8') as f:
            for linha in f:
                partes = linha.strip().split(',')
                if len(partes) != 3:
                    continue
                lugar, no_serie, prim = partes
                grupo = self.assentos_por_aviao.setdefault(no_serie, {True: [], False: []})
                grupo[prim.strip().lower() == 'true'].append(lugar)
        
    def gerar_voos(self):
        """Voos de 1 Jan a 31 Jul 2025, agendados POR DIA de calendário:
        os 10 aviões, várias pernas de manhã à noite, descanso à noite."""
        print("Gerando voos...")
        data_inicio = (self.data_hoje - timedelta(days=120)).replace(hour=0, minute=0, second=0, microsecond=0)
        data_fim = self.data_hoje + timedelta(days=90)
        posicao = {aviao: random.choice(AEROPORTOS) for aviao in AVIOES}  # onde cada avião está parado
        usados = set()                       # (hora_partida, partida, chegada) p/ o UNIQUE

        dia = data_inicio
        while dia <= data_fim:
            for aviao in AVIOES:             # <-- TODOS os aviões (antes só 5)
                hora = dia.replace(hour=random.randint(6, 9),
                                    minute=random.randint(0, 59), second=random.randint(0, 59))
                for _ in range(random.randint(*PERNAS_POR_DIA)):
                    origem = posicao[aviao]
                    destino = random.choice([a for a in AEROPORTOS if a != origem])
                    chegada = hora + timedelta(minutes=random.randint(90, 240))
                    if chegada.date() != dia.date() or chegada.hour >= 23:   # não voa de noite
                        break
                    chave = (hora, origem, destino)
                    if chave in usados:
                        break
                    usados.add(chave)
                    self.voos.append({'id': self.contador_voo, 'no_serie': aviao,
                        'hora_partida': hora, 'hora_chegada': chegada,
                        'partida': origem, 'chegada': destino})
                    self.contador_voo += 1
                    posicao[aviao] = destino                 # continuidade: fica no destino
                    hora = chegada + timedelta(minutes=random.randint(45, 120))  # turnaround
                    if hora.hour >= 21:
                        break
            dia += timedelta(days=1)
        print(f"Gerados {len(self.voos)} voos")
        
    def gerar_vendas(self):
        """Gera mais de 10.000 vendas"""
        print("Gerando vendas...")
        # vendas de ~8 meses atrás até agora (sempre antes dos voos que reservam)
        venda_inicio = (self.data_hoje - timedelta(days=240)).replace(hour=0, minute=0, second=0, microsecond=0)
        total_dias = (self.data_hoje - venda_inicio).days
        for i in range(NUM_VENDAS):
            nif = f"{100000000 + (i % 900000000):09d}"
            dias = (i * total_dias) // NUM_VENDAS
            hora_venda = venda_inicio + timedelta(days=dias,
                                    hours=random.randint(8, 22), minutes=random.randint(0, 59))

            balcao = random.choice(AEROPORTOS)
            venda = {
                'codigo_reserva': self.contador_venda,
                'nif_cliente': nif,
                'balcao': balcao,
                'hora': hora_venda
            }
            self.vendas.append(venda)
            self.contador_venda += 1
        print(f"Geradas {len(self.vendas)} vendas")

    
    def obter_assentos_disponiveis(self, voo_id, aviao, primeira_classe):
        ocupados = self.assentos_ocupados.setdefault(voo_id, set())
        return [s for s in self.assentos_por_aviao[aviao][primeira_classe] if s not in ocupados]
    
    def gerar_bilhetes(self):
        print("Gerando bilhetes...")
        vendas_ord = sorted(self.vendas, key=lambda v: v['hora'])     # ordena UMA vez
        horas = [v['hora'] for v in vendas_ord]
        for voo in sorted(self.voos, key=lambda x: x['hora_partida']):
            aviao = voo['no_serie']
            idx = bisect.bisect_left(horas, voo['hora_partida'])       # vendas anteriores ao voo (corte O(log n))
            if idx == 0:
                continue
            n_total = random.randint(*BILHETES_POR_VOO)
            n_p = max(1, n_total // 8); n_e = max(1, n_total - n_p)
            checkin = voo['hora_partida'] < self.data_hoje
            usados_pass = set()                          # (reserva, nome) p/ UNIQUE(voo,reserva,nome)
            for prim, n, base in ((True, n_p, 850 if aviao == 'SN004' else 450),
                                  (False, n_e, 280 if aviao == 'SN004' else 180)):
                for _ in range(n):
                    venda = vendas_ord[random.randrange(idx)]; nome = random.choice(NOMES_PASSAGEIROS)
                    tent = 0
                    while (venda['codigo_reserva'], nome) in usados_pass and tent < 5:
                        venda = vendas_ord[random.randrange(idx)]; nome = random.choice(NOMES_PASSAGEIROS); tent += 1
                    if (venda['codigo_reserva'], nome) in usados_pass:
                        continue
                    usados_pass.add((venda['codigo_reserva'], nome))
                    no_serie_b = aviao
                    lugar = None
                    if checkin:                          # check-in só p/ voos já realizados
                        disp = self.obter_assentos_disponiveis(voo['id'], aviao, prim)
                        if disp:
                            lugar = random.choice(disp); self.assentos_ocupados[voo['id']].add(lugar)
                    self.bilhetes.append({'id': self.contador_bilhete, 'voo_id': voo['id'],
                        'codigo_reserva': venda['codigo_reserva'], 'nome_passegeiro': nome,
                        'preco': f"{base + random.randint(0, 200):.2f}", 'prim_classe': prim,
                        'lugar': lugar, 'no_serie': no_serie_b})
                    self.contador_bilhete += 1
        print(f"Gerados {len(self.bilhetes)} bilhetes")

    
    def gerar_sql_inserts(self):
        out = []
        def bloco(titulo, cabecalho, linhas):
            if not linhas:
                return                       # <-- guarda: nunca gera "VALUES ;" inválido
            out.append(f"-- ===== {titulo} =====")
            for k in range(0, len(linhas), 1000):        # lotes de 1000
                out.append(cabecalho)
                out.append(",\n".join(linhas[k:k+1000]) + ";")
            out.append("")
        bloco("VOO", "INSERT INTO voo (no_serie, hora_partida, hora_chegada, partida, chegada) VALUES",
              [f"('{v['no_serie']}', '{v['hora_partida']:%Y-%m-%d %H:%M:%S}', '{v['hora_chegada']:%Y-%m-%d %H:%M:%S}', '{v['partida']}', '{v['chegada']}')" for v in self.voos])
        bloco("VENDA", "INSERT INTO venda (nif_cliente, balcao, hora) VALUES",
              [f"('{s['nif_cliente']}', '{s['balcao']}', '{s['hora']:%Y-%m-%d %H:%M:%S}')" for s in self.vendas])
        bilh = []
        for b in self.bilhetes:
            lugar = f"'{b['lugar']}'" if b['lugar'] else "NULL"
            nos = f"'{b['no_serie']}'" if b['no_serie'] else "NULL"
            nome = b['nome_passegeiro'].replace("'", "''")     # escapa aspas (defensivo)
            bilh.append(f"({b['voo_id']}, {b['codigo_reserva']}, '{nome}', {b['preco']}, {b['prim_classe']}, {lugar}, {nos})")
        bloco("BILHETE", "INSERT INTO bilhete (voo_id, codigo_reserva, nome_passegeiro, preco, prim_classe, lugar, no_serie) VALUES", bilh)
        return "\n".join(out)

    
    def gerar_csv_files(self):
        """Gera arquivos CSV para cada tabela"""
        files = {}
        
        # CSV para voos
        voos_csv = StringIO()
        voos_writer = csv.writer(voos_csv)
        voos_writer.writerow(['id','no_serie', 'hora_partida', 'hora_chegada', 'partida', 'chegada'])
        for voo in self.voos:
            voos_writer.writerow([
                voo['id'],
                voo['no_serie'],
                voo['hora_partida'].strftime('%Y-%m-%d %H:%M:%S'),
                voo['hora_chegada'].strftime('%Y-%m-%d %H:%M:%S'),
                voo['partida'],
                voo['chegada']
            ])
        files['voos.csv'] = voos_csv.getvalue()
        
        # CSV para vendas
        vendas_csv = StringIO()
        vendas_writer = csv.writer(vendas_csv)
        vendas_writer.writerow(['codigo_reserva','nif_cliente', 'balcao', 'hora'])
        for venda in self.vendas:
            vendas_writer.writerow([
                venda['codigo_reserva'],
                venda['nif_cliente'],
                venda['balcao'],
                venda['hora'].strftime('%Y-%m-%d %H:%M:%S')
            ])
        files['vendas.csv'] = vendas_csv.getvalue()
        
        # CSV para bilhetes
        bilhetes_csv = StringIO()
        bilhetes_writer = csv.writer(bilhetes_csv)
        bilhetes_writer.writerow(['id', 'voo_id', 'codigo_reserva', 'nome_passegeiro', 'preco', 'prim_classe', 'lugar', 'no_serie'])
        for bilhete in self.bilhetes:
            bilhetes_writer.writerow([
                bilhete['id'],
                bilhete['voo_id'],
                bilhete['codigo_reserva'],
                bilhete['nome_passegeiro'],
                bilhete['preco'],
                bilhete['prim_classe'],
                bilhete['lugar'] or '',
                bilhete['no_serie'] or ''
            ])
        files['bilhetes.csv'] = bilhetes_csv.getvalue()
        
        return files

   
    def gerar_txt_files(self):
        """Escreve Voos/Vendas/Bilhetes.txt no formato do \\copy do populate.sql:
        sem cabecalho, delimitador ',', \\N para NULL (lugar sem check-in)."""
        files = {}

        files['Voos.txt'] = "".join(
            f"{v['id']},{v['no_serie']},"
            f"{v['hora_partida']:%Y-%m-%d %H:%M:%S},{v['hora_chegada']:%Y-%m-%d %H:%M:%S},"
            f"{v['partida']},{v['chegada']}\n"
            for v in self.voos)

        files['Vendas.txt'] = "".join(
            f"{s['codigo_reserva']},{s['nif_cliente']},{s['balcao']},{s['hora']:%Y-%m-%d %H:%M:%S}\n"
            for s in self.vendas)

        linhas = []
        for b in self.bilhetes:
            lugar = b['lugar'] if b['lugar'] is not None else '\\N'
            no_serie = b['no_serie'] if b['no_serie'] is not None else '\\N'
            linhas.append(
                f"{b['id']},{b['voo_id']},{b['codigo_reserva']},{b['nome_passegeiro']},"
                f"{b['preco']},{b['prim_classe']},{lugar},{no_serie}\n")
        files['Bilhetes.txt'] = "".join(linhas)

        return files
 
    def gerar_estatisticas(self):
        """Gera estatísticas dos dados gerados"""
        stats = []
        
        # Estatísticas de voos
        voos_por_aeroporto = {}
        voos_por_aviao = {}
        for voo in self.voos:
            voos_por_aeroporto[voo['partida']] = voos_por_aeroporto.get(voo['partida'], 0) + 1
            voos_por_aviao[voo['no_serie']] = voos_por_aviao.get(voo['no_serie'], 0) + 1
        
        stats.append("=== ESTATÍSTICAS DOS DADOS GERADOS ===")
        stats.append(f"Total de voos: {len(self.voos)}")
        stats.append(f"Voos por aeroporto: {dict(sorted(voos_por_aeroporto.items()))}")
        stats.append(f"Voos por avião: {dict(sorted(voos_por_aviao.items()))}")
        stats.append("")
        
        # Estatísticas de vendas
        stats.append(f"Total de vendas: {len(self.vendas)}")
        stats.append("")
        
        # Estatísticas de bilhetes
        bilhetes_primeira = sum(1 for b in self.bilhetes if b['prim_classe'])
        bilhetes_economica = len(self.bilhetes) - bilhetes_primeira
        bilhetes_com_checkin = sum(1 for b in self.bilhetes if b['lugar'])
        
        stats.append(f"Total de bilhetes: {len(self.bilhetes)}")
        stats.append(f"Bilhetes primeira classe: {bilhetes_primeira}")
        stats.append(f"Bilhetes classe econômica: {bilhetes_economica}")
        stats.append(f"Bilhetes com check-in: {bilhetes_com_checkin}")
        
        return "\n".join(stats)

def main():
    print("=== GERADOR DE DADOS PARA COMPANHIA AÉREA ===")
    print("Gerando dados que respeitam todas as restrições de integridade...")
    print()
    
    gerador = GeradorDados()
    
    # Gerar todos os dados
    gerador.gerar_voos()
    gerador.gerar_vendas()
    gerador.gerar_bilhetes()
    
    print()
    print("=== DADOS GERADOS COM SUCESSO ===")
    print()
    
    # Mostrar estatísticas
    print(gerador.gerar_estatisticas())
    print()
    
    # Gerar arquivos
    print("Gerando arquivos...")
    
    # Salvar SQL
    sql_content = gerador.gerar_sql_inserts()
    with open('dados_companhia_aerea.sql', 'w', encoding='utf-8') as f:
        f.write(sql_content)
    print("✓ Arquivo SQL gerado: dados_companhia_aerea.sql")
    
    # Salvar CSVs
    csv_files = gerador.gerar_csv_files()
    for filename, content in csv_files.items():
        with open(filename, 'w', encoding='utf-8') as f:
            f.write(content)
        print(f"✓ Arquivo CSV gerado: {filename}")

    # Salvar TXT (formato do \copy do populate.sql — dados novos p/ o populate.sql)
    txt_files = gerador.gerar_txt_files()
    for filename, content in txt_files.items():
        with open(filename, 'w', encoding='utf-8') as f:
            f.write(content)
        print(f"✓ Arquivo TXT gerado: {filename}")
    
    print()
    print("=== CONCLUÍDO ===")
    print("Arquivos gerados:")
    print("- dados_companhia_aerea.sql (comandos INSERT)")
    print("- voos.csv (dados dos voos)")
    print("- vendas.csv (dados das vendas)")
    print("- bilhetes.csv (dados dos bilhetes)")
    print()
    print("Pode agora revisar os dados e executar o SQL na sua base de dados!")

if __name__ == "__main__":
    main()
