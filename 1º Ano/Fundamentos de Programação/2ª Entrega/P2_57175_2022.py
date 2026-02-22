# TAD gerador

def cria_gerador(b,s):
    """
    Recebe um inteiro b correspondente ao número de bits
    do gerador e um inteiro positivo s correspondente à seed ou estado inicial,
    e devolve o gerador correspondente.
    """
    if type(b) != int or type(s) != int or b not in (32, 64) or s <= 0 or s > 2**b:
        raise ValueError('cria_gerador: argumentos invalidos')
    return [b, s]

def cria_copia_gerador(g):
    """
    recebe um gerador e devolve uma cópia nova do gerador.
    """
    return g.copy()


def obtem_estado(g):
    """
    devolve o estado atual do gerador g sem o alterar.
    """

    return g[1]

def define_estado(g,s):
    """
    define o novo valor do estado do gerador g como sendo s,
    e devolve s.
    """
    g[1] = s
    return s
   
def atualiza_estado(g):
    """
    atualiza o estado do gerador g de acordo com o algoritmo
    xorshift de geraçãoo de números pseudoaleatórios, e devolve-o.

    """
    s = g[1]
    if g[0] == 32:
        s ^= ( s << 13 ) & 0xFFFFFFFF
        s ^= ( s >> 17 ) & 0xFFFFFFFF
        s ^= ( s << 5 ) & 0xFFFFFFFF
    elif g[0] == 64:
        s ^= ( s << 13 ) & 0xFFFFFFFFFFFFFFFF  
        s ^= ( s >> 7 ) & 0xFFFFFFFFFFFFFFFF 
        s ^= ( s << 17 ) & 0xFFFFFFFFFFFFFFFF
        
    return define_estado(g, s)

def eh_gerador(arg):
    """
    devolve True caso o seu argumento seja um TAD gerador e
    False caso contrário
    """
    return type(arg) == list and len(arg) == 2 and \
           type(arg[0]) == int and type(arg[1]) == int and \
           arg[0] in (32, 64) and 0 < arg[1] <= 2**arg[0]

def geradores_iguais(g1,g2):
    """
    devolve True apenas se g1 e g2 são geradores e são
    iguais.

    """
    return eh_gerador(g1) and eh_gerador(g2) and g1 == g2

def gerador_para_str(g):
    """
    devolve a cadeia de carateres que representa o seu argu￾mento
    """

    def int_to_str(i):
        return str(i)
    return 'xorshift{}(s={})'.format(int_to_str(g[0]),int_to_str(g[1]))


def gera_numero_aleatorio(g,n):
    """
    atualiza o estado do gerador g e devolve um número aleatório no 
    intervalo [1, n] obtido a partir do novo estado s de g como 1 + mod(s,n),
    em que mod() corresponde à operação resto da divisão inteira.
    """
    return  (atualiza_estado(g) % n) + 1


def gera_carater_aleatorio(g,c):
    """
    atualiza o estado do gerador g e devolve um caráter
    aleatório no intervalo entre 'A' e o caráter maiúsculo c. Este é obtido a partir do
    novo estado s de g como o caráter na posição mod(s, l) da cadeia de carateres
    de tamanho l formada por todos os carateres entre 'A' e c. A operação mod()
    corresponde ao resto da divisão inteira.   
    """

    return chr(gera_numero_aleatorio(g,(ord(c)+1-ord('A'))) + ord('A')-1)

# TAD coordenada

def cria_coordenada(col,lin):
    """
    recebe os valores correspondentes à coluna col e
    linha lin e devolve a coordenada correspondente.
    """

    if type(col) == str and type(lin) == int and 'A' <= col <= 'Z' and 1 <= lin <= 99 and len(col)==1 :
        return (col,lin)
    else:
        raise ValueError('cria_coordenada: argumentos invalidos')

def obtem_coluna(c):
    """
    devolve a coluna col da coordenada c
    """

    return c[0]

def obtem_linha(c):
    """
    devolve a linha lin da coordenada c.
    """
    return c[1]

def eh_coordenada(arg):
    """
    devolve True caso o seu argumento seja um TAD coordenada e False caso contrário.
    """
    return type(arg) == tuple and len(arg) == 2 and \
           type(arg[0]) == str and len(arg[0]) == 1 and 'A' <= arg[0] <= 'Z' and \
           type(arg[1]) == int and 1 <= arg[1] <= 99

def coordenadas_iguais(c1,c2):
    """
    devolve True apenas se c1 e c2 são coordenadas e são iguais.
    """
    return eh_coordenada(c1) and eh_coordenada(c2) and c1 == c2

def coordenada_para_str(c):
    """
    devolve a cadeia de carateres que representa o seu argumento
    """
    def int_to_str(i):
        return '0'+ str(i) if i< 10 else str(i)
    return '{}{}'.format(c[0],int_to_str(c[1]))

def str_para_coordenada(s):
    """
    devolve a coordenada reapresentada pelo seu argumento.
    """
    return (s[0], int(s[1:]))
    

def obtem_coordenadas_vizinhas(c):
    """
    devolve um tuplo com as coordenadas vizinhas à coordenada c,
    começando pela coordenada na diagonal acima-esquerda de c e seguindo no sentido horário.
    """
    col = ord(c[0])
    lin = c[1]
    
    # Ordem exigida: Cima-Esq, Cima, Cima-Dir, Dir, Baixo-Dir, Baixo, Baixo-Esq, Esq
    deltas = ((-1, -1), (0, -1), (1, -1), (1, 0), (1, 1), (0, 1), (-1, 1), (-1, 0))
    
    vizinhas = []
    for dc, dl in deltas:
        nova_col = chr(col + dc)
        nova_lin = lin + dl
        if 'A' <= nova_col <= 'Z' and 1 <= nova_lin <= 99:
            vizinhas.append((nova_col, nova_lin))
            
    return tuple(vizinhas)

def obtem_coordenada_aleatoria(c,g):
    """
    recebe uma coordenada c e um TAD gerador g,
    e devolve uma coordenada gerada aleatoriamente em
    que c define a maior coluna e maior linha possíveis.
    """
    return (gera_carater_aleatorio(g,obtem_coluna(c)),gera_numero_aleatorio(g,obtem_linha(c)))

# TAD parcela

def cria_parcela():
    """
    devolve uma parcela tapada sem mina escondida.
    """
    return ['#',0]

def cria_copia_parcela(p):
    """
    recebe uma parcela p e devolve uma nova cópia da parcela
    """
    
    return p.copy()

def limpa_parcela(p):
    """
    modifica destrutivamente a parcela p modificando o seu estado para limpa, e devolve a própria parcela.
    """
    p[0] = '?'
    return p

def marca_parcela(p):
    """
    modifica destrutivamente a parcela p modificando o seu
    estado para marcada com uma bandeira, e devolve a própria parcela.
    """
    
    p[0] = '@'

    return p

def desmarca_parcela(p):
    """
    modifica destrutivamente a parcela p modificando o seu
    estado para tapada, e devolve a própria parcela.
    """
    p[0] = '#'
    return p

def esconde_mina(p):
    """
    modifica destrutivamente a parcela p escondendo uma mina
    na parcela, e devolve a própria parcela.
    """
    p[1] = 'X'
    return p

def eh_parcela(arg):
    """
    devolve True caso o seu argumento seja um TAD parcela e
    False caso contrário
    """
    return type(arg) == list and len(arg) == 2 and arg[0] in ('#', '?', '@') and arg[1] in (0, 'X')

def eh_parcela_tapada(p):
    """
    devolve True caso a parcela p se encontre tapada e False
    caso contrário.
    """
    return eh_parcela(p) and p[0] == '#'

def eh_parcela_marcada(p):
    """
    devolve True caso a parcela p se encontre marcada
    com uma bandeira e False caso contrário.
    """
    return eh_parcela(p) and p[0] == '@'

def eh_parcela_limpa(p):
    """
    devolve True caso a parcela p se encontre limpa e False
    caso contrário.
    """
    return eh_parcela(p) and p[0] == '?'

def eh_parcela_minada(p):
    """
    devolve True caso a parcela p esconda uma mina e
    False caso contrário.
    """
    return eh_parcela(p)  and p[1] == 'X'

def parcelas_iguais(p1,p2):
    """
    devolve True apenas se p1 e p2 são parcelas e são iguais.
    """
    return eh_parcela(p1) and eh_parcela(p2) and p1 == p2

def parcela_para_str(p):
    """
    devolve a cadeia de caracteres que representa a parcela
    em função do seu estado: parcelas tapadas ('#'), parcelas marcadas ('@'),
    parcelas limpas sem mina ('?') e parcelas limpas com mina ('X').
    """
    if eh_parcela_limpa(p) and eh_parcela_minada(p):
        return p[1]
    else:    
        return p[0]

def alterna_bandeira(p):
    """
    recebe uma parcela p e modifica-a destrutivamente da seguinte
    forma: desmarca se estiver marcada e marca se estiver tapada, devolvendo True.
    Em qualquer outro caso, não modifica a parcela e devolve False 
    """
    if eh_parcela_marcada(p):
        desmarca_parcela(p)
        return True
    elif eh_parcela_tapada(p):
        marca_parcela(p)
        return True
    return False

    
# TAD campo

def cria_campo(c,l):
    """
    recebe uma cadeia de carateres e um inteiro correspondentes
    à última coluna e à última linha de um campo de minas, e devolve o campo
    do tamanho pretendido formado por parcelas tapadas sem minas
    """

    if type(c) == str and type(l) == int and 'A' <= c <= 'Z' and 1 <= l <= 99 and len(c)==1:
        campo = {'colunas': '' , 'linhas': {}}
        i = 'A'
        while i <= c:
            campo['colunas'] += i
            i = chr(ord(i)+1)
        j = 1
        
        while j <= l:
            parcelas = []
            a=1
            while a <= len(campo['colunas']):
                parcelas += [cria_parcela()]
            
                a +=1
            campo['linhas'][j] = parcelas    
            j +=1
        return campo
    else:
        raise ValueError('cria_campo: argumentos invalidos')    



def cria_copia_campo(m):
    """
    recebe um campo e devolve uma nova cópia do campo
    """
    novo_m = {'colunas': m['colunas'], 'linhas': {}}
    for linha in m['linhas']:
        novo_m['linhas'][linha] = []
        for parcela in m['linhas'][linha]:
            # Copiar cada parcela individualmente para nova memória
            novo_m['linhas'][linha].append([parcela[0], parcela[1]])
    return novo_m

def obtem_ultima_coluna(m):
    """
    devolve a cadeia de caracteres que corresponde à
    última coluna do campo de minas.
    """

    return m['colunas'][-1]

def obtem_ultima_linha(m):
    """
    devolve o valor inteiro que corresponde à
    última linha do campo de minas.
    """
    res = ()
    for i in m['linhas']:
        res += (i,) 

    return res[-1]

def obtem_parcela(m,c):
    """
    devolve a parcela do campo m que se encontra na coordenada c.
    """
    col = ord(c[0])-ord('A')
    return m['linhas'][c[1]][col]



def obtem_coordenadas(m,s):
    """
    devolve o tuplo formado pelas coordenadas ordenadas em ordem ascendente de esquerda
    à direita e de cima a baixo das parcelas dependendo do valor de s: 'limpas' para as
    parcelas limpas, 'tapadas' para as parcelas tapadas, 'marcadas' para as parcelas
    marcadas, e 'minadas' para as parcelas que escondem minas.
    """
    res = ()
    
    if s == 'limpas':
        for i in m['linhas']:
           for j in range(len(m['linhas'][i])):
            if eh_parcela_limpa(m['linhas'][i][j]):
                res += (cria_coordenada(m['colunas'][j],i),)
                
        
    if s == 'tapadas':
        for i in m['linhas']:
           for j in range(len(m['linhas'][i])):
            if eh_parcela_tapada(m['linhas'][i][j]):
                res += (cria_coordenada(m['colunas'][j],i),)
                
        
    if s == 'marcadas':
        for i in m['linhas']:
           for j in range(len(m['linhas'][i])):
            if eh_parcela_marcada(m['linhas'][i][j]):
                res += (cria_coordenada(m['colunas'][j],i),)
                
        
    if s == 'minadas':
        for i in m['linhas']:
           for j in range(len(m['linhas'][i])):
            if eh_parcela_minada(m['linhas'][i][j]):
                res += (cria_coordenada(m['colunas'][j],i),)
                
    return res
    
def obtem_numero_minas_vizinhas(m,c):
    """
    devolve o número de parcelas vizinhas da parcela na coordenada c que escondem uma mina.
    """
    vizinhas = obtem_coordenadas_vizinhas(c)
    aux =[]
    for i in vizinhas:
        if i[0] in m['colunas'] and i[1] in m['linhas']:
            aux += [obtem_parcela(m,i)]
  
    return len(list(filter(lambda x : eh_parcela_minada(x) ,aux)))

def eh_campo(arg):
    """
    devolve True caso o seu argumento seja um TAD campo e False caso contrário.
    """
    
    return type(arg) == dict and len(arg) == 2 and 'colunas' in arg and 'linhas' in arg and type(arg['colunas']) == str and type(arg['linhas']) == dict

def eh_coordenada_do_campo(m,c):
    """
    devolve True se c é uma coordenada válida dentro do campo m.
    """
    return eh_campo(m) and eh_coordenada(c) and c[0] <= obtem_ultima_coluna(m) and c[1] <= obtem_ultima_linha(m)

def campos_iguais(m1,m2):
    """
    devolve True apenas se m1 e m2 forem campos e forem iguais.
    """
    return eh_campo(m1) and eh_campo(m2) and m1 == m2

def campo_para_str(m):
    """
    devolve uma cadeia de caracteres que representa o campo de minas
    """
    def int_to_str(i):
        return '0'+ str(i) if i < 10 else str(i)
        
    res ='   {}\n  +{}+'.format(m['colunas'], '-'*len(m['colunas']))
    
    for l in m['linhas']:
        res +='\n{}|'.format(int_to_str(l))
        for col_char in m['colunas']:
            c = cria_coordenada(col_char, l)
            p = obtem_parcela(m, c)
            
            # Se a parcela está limpa e não é uma mina detornada
            if eh_parcela_limpa(p) and not eh_parcela_minada(p):
                minas_viz = obtem_numero_minas_vizinhas(m, c)
                if minas_viz > 0:
                    res += str(minas_viz)
                else:
                    res += ' '
            else:
                res += parcela_para_str(p)
        res += '|'    
    res += '\n  +{}+'.format('-'*len(m['colunas']))
   
    return res  
   

def coloca_minas(m,c,g,n):
    """
    modifica destrutivamente o campo m escondendo n minas em parcelas dentro do campo.
    As n coordenadas são geradas em sequência utilizando o gerador g, de modo a que não
    coincidam com a coordenada c nem com nenhuma parcela vizinha desta, nem se sobreponham
    com minas colocadas anteriormente.
    """
    vizinhas = obtem_coordenadas_vizinhas(c)
    i = 1
    
    
    while i <= n:
        c_al = obtem_coordenada_aleatoria((obtem_ultima_coluna(m),obtem_ultima_linha(m)),g)
        
        
        if not coordenadas_iguais(c,c_al) and c_al not in vizinhas and (c_al[0] in m['colunas'] and c_al[1] in m['linhas']):
            if not eh_parcela_minada(obtem_parcela(m,c_al)):               
                m['linhas'][c_al[1]][ord(c_al[0])-ord('A')] = esconde_mina(obtem_parcela(m,c_al))
             
                i += 1        
    return m

def limpa_campo(m, c):
    """
    modifica destrutivamente o campo limpando a parcela na coordenada c.
    Se não houver nenhuma mina vizinha, limpa iterativamente todas as parcelas vizinhas tapadas.
    """
    p = obtem_parcela(m, c)
    
    # Se a parcela já está limpa, não fazemos nada
    if eh_parcela_limpa(p):
        return m
        
    # Limpar a parcela atual
    limpa_parcela(p)
    
    # Só limpamos as vizinhas em cadeia se não for uma mina e o nº de minas vizinhas for 0
    if not eh_parcela_minada(p) and obtem_numero_minas_vizinhas(m, c) == 0:
        for viz in obtem_coordenadas_vizinhas(c):
            if eh_coordenada_do_campo(m, viz):
                if eh_parcela_tapada(obtem_parcela(m, viz)):
                    limpa_campo(m, viz) # Chamada recursiva para espalhar a limpeza
                    
    return m                  
    
# Funções adicionais

def jogo_ganho(m):
    """
    é uma função auxiliar que recebe um campo do jogo das minas e devolve
    True se todas as parcelas sem minas se encontram limpas, ou False caso contrário
    """
    area_total = len(m['colunas']) * len(m['linhas'])
    limpas = len(obtem_coordenadas(m, 'limpas'))
    minas = len(obtem_coordenadas(m, 'minadas'))
    return limpas == area_total - minas

def turno_jogador(m):
    """
    é uma funçãao auxiliar que recebe um campo de minas e oferece ao jogador
    a opção de escolher uma ação e uma coordenada. A função modifica destrutivamente
    o campo de acordo com ação escolhida, devolvendo False caso o jogador tenha limpo
    uma parcela que continha uma mina, ou True caso contrário.
    """
    acao = input('Escolha uma ação, [L]impar ou [M]arcar:')
    while acao not in ('L', 'M'):
        acao = input('Escolha uma ação, [L]impar ou [M]arcar:')
    
    col_max = m['colunas'][-1]
    lin_max = len(m['linhas'])
    
    # Pede e valida a coordenada
    coord_str = input('Escolha uma coordenada:')
    while not (len(coord_str) == 3 and 'A' <= coord_str[0] <= col_max and \
               coord_str[1:].isdigit() and 1 <= int(coord_str[1:]) <= lin_max):
        coord_str = input('Escolha uma coordenada:')
            
    coord = str_para_coordenada(coord_str)
    
    # Executa a jogada
    if acao == 'M':
        alterna_bandeira(obtem_parcela(m, coord))
        return True # Marcar nunca rebenta minas, o jogo continua (True)
    elif acao == 'L':
        limpa_campo(m, coord)
        # Se a parcela tiver uma mina, explode = Game Over (False)
        # Se não tiver mina = O jogo continua (True)
        return not eh_parcela_minada(obtem_parcela(m, coord))

  
                
        

def minas(c, l, n, d, s):
    # 1. Validação estrita dos argumentos
    if type(c) != str or len(c) != 1 or c < 'A' or c > 'Z' or \
       type(l) != int or l < 1 or l > 99 or \
       type(d) != int or d not in (32, 64) or \
       type(s) != int or s <= 0 or s > 2**d:
        raise ValueError('minas: argumentos invalidos')
    
    # Validação do número de minas (n) 
    area = (ord(c) - ord('A') + 1) * l
    if type(n) != int or n < 1 or n > area - 9:
        raise ValueError('minas: argumentos invalidos')
        
    # 2. Inicialização do jogo
    m = cria_campo(c, l)
    g = cria_gerador(d, s)
    
    estado = 'jogar'
    primeira_limpeza = True
    
    # 3. Ciclo principal do jogo
    while estado == 'jogar':
        # Imprime o estado atual do campo
        bandeiras = len(obtem_coordenadas(m, 'marcadas'))
        print(f'   [Bandeiras {bandeiras}/{n}]')
        print(campo_para_str(m))
        
        # Na primeira jogada, a ação é automaticamente 'L' e não é perguntada
        if primeira_limpeza:
            acao = 'L'
        else:
            # Pede a ação ao jogador
            acao = input('Escolha uma ação, [L]impar ou [M]arcar:')
            while acao not in ('L', 'M'):
                acao = input('Escolha uma ação, [L]impar ou [M]arcar:')
        
        # Pede a coordenada ao jogador
        coord_str = input('Escolha uma coordenada:')
        # Garante que a coordenada tem tamanho 3, começa com letra válida e acaba com número válido
        while not (len(coord_str) == 3 and 'A' <= coord_str[0] <= c and \
                   coord_str[1:].isdigit() and 1 <= int(coord_str[1:]) <= l):
            coord_str = input('Escolha uma coordenada:')
            
        coord = str_para_coordenada(coord_str)
        
        # Executa a jogada
        if acao == 'M':
            alterna_bandeira(obtem_parcela(m, coord))
        elif acao == 'L':
            # Se for o primeiro clique de limpeza, colocamos as minas agora!
            if primeira_limpeza:
                coloca_minas(m, coord, g, n)
                primeira_limpeza = False
            
            limpa_campo(m, coord)
            
            # Se limpou uma mina, perdeu
            if eh_parcela_minada(obtem_parcela(m, coord)):
                estado = 'derrota'
        
        # Verifica a condição de vitória
        if estado == 'jogar' and len(obtem_coordenadas(m, 'limpas')) == area - n:
            estado = 'vitoria'
            
    # 4. Fim do Jogo
    bandeiras = len(obtem_coordenadas(m, 'marcadas'))
    print(f'   [Bandeiras {bandeiras}/{n}]')
    print(campo_para_str(m))
    
    if estado == 'vitoria':
        print('VITORIA!!!')
        return True
    else:
        print('BOOOOOOOM!!!')
        return False