# TAD gerador

def cria_gerador(b,s):
    """
    Recebe um inteiro b correspondente ao número de bits
    do gerador e um inteiro positivo s correspondente à seed ou estado inicial,
    e devolve o gerador correspondente.
    """
    if (b == 32 or b == 64) and 0 < s <= 2**b and type(b) == int and type(s) == int:
        return [b,s]
    else:
        raise ValueError('cria_gerador: argumentos invalidos')

def cria_copia_gerador(g):
    """
    recebe um gerador e devolve uma cópia nova do gerador.
    """
    return g.copy()


def obtem_estado(g):
    """
    devolve o estado atual do gerador g sem o alterar.
    """

    s = g[1]
    if g[0] == 32:
        s ^= ( s << 13 ) & 0xFFFFFFFF
        s ^= ( s >> 17 ) & 0xFFFFFFFF
        s ^= ( s << 5 ) & 0xFFFFFFFF
        return s
    if g[0] == 64:
        s ^= ( s << 13 ) & 0xFFFFFFFFFFFFFFFF  
        s ^= ( s >> 7 ) & 0xFFFFFFFFFFFFFFFF 
        s ^= ( s << 17 ) & 0xFFFFFFFFFFFFFFFF
        return s  

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
    return define_estado(g,obtem_estado(g))

def eh_gerador(arg):
    """
    devolve True caso o seu argumento seja um TAD gerador e
    False caso contrário
    """
    return type(arg) == list and len(arg) == 2 and (arg(0) == 32 or arg(0) == 64) and 0 < arg(1) <= 2**arg(0)

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
    return type(arg) == tuple and len(arg) == 2 and 'A' <= arg[0] <= 'Z' and 1 <= arg[1] <= 99

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
    if int(s[1]) == 0:
        return (s[0],int(s[2]))
    else:
        (s[0],int(s[1:]))
    

def obtem_coordenadas_vizinhas(c):
    """
    devolve um tuplo com as coordenadas vizinhas à coordenada c,
    começando pela coordenada na diagonal acima-esquerda de c e seguindo no sentido horário.
    """
    return tuple ((chr(ord(c[0]) + dc), c[1]  + dl) for dc, dl in ((1,-1), (1,0), (1,1), (0,1), (-1,1),(-1,0),(-1,-1),(0,-1)) if 'A' <= chr(ord(c[0]) + dc)<= 'Z' and 1 <= (c[1]  + dl) <=99)

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
    return type(arg) == list and len(arg) == 2 and  (arg[0] == '#' or arg[0] == '?' or arg[0] == '@' or arg[0] == ' ' or (type(arg[0]) == int and arg[0] > 0) or arg[1] == 0 or arg[1] == 'X')

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
    return eh_parcela(p) and (p[0] == '?' or p[0] == ' ' or (type(p[0]) == int and p[0] > 0 ))

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
    if eh_parcela_tapada(p) or eh_parcela_minada(p):
         
        return marca_parcela(p) 
        
    if eh_parcela_marcada(p):
        
        return desmarca_parcela(p)  
    
    return p[0] == '#' or p[0] == '@'

    
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
    return m.copy()

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
    limpas = obtem_coordenadas(m,'limpas')
    
    aux = ()
    for i in limpas:
        aux += (i,)
    
    for z in aux:
        if obtem_numero_minas_vizinhas(m,z) > 0:
            obtem_parcela(m,z)[0] = obtem_numero_minas_vizinhas(m,z)
        else:
            obtem_parcela(m,z)[0] = ' '
        

    def int_to_str(i):
        return '0'+ str(i) if i< 10 else str(i)
    res ='   {}\n  +{}+'.format(m['colunas'],'-'*len(m['colunas']))
    
    for i in m['linhas']:
        j=0
        res +='\n{}|'.format(int_to_str(i))
        while j <  len(m['linhas'][i]):
            res +=   '{}'.format(m['linhas'][i][j][0])
            j += 1
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

def limpa_campo(m,c):
    """
    modifica destrutivamente o campo limpando a parcela na coordenada c e o devolvendo-a.
    Se não houver nenhuma mina vizinha escondida, limpa iterativamente todas as parcelas
    vizinhas tapadas. Caso a parcela se encontre já limpa, a operação não tem efeito.
    """
        
    vizinhas = obtem_coordenadas_vizinhas(c)
    
    j=0
    while j < len(vizinhas):
            if  vizinhas[j][0] in m['colunas'] and vizinhas[j][1] in m['linhas']:
                if vizinhas[j] in obtem_coordenadas(m,'tapadas') and vizinhas[j] not in obtem_coordenadas(m,'minadas'):
                   m['linhas'][vizinhas[j][1]][ord(vizinhas[j][0])-ord('A')] = limpa_parcela(obtem_parcela(m,vizinhas[j])) 
                   
                   m = limpa_campo(m,vizinhas[j])
                   j +=1
                elif vizinhas[j] in obtem_coordenadas(m,'minadas'):
                    
                    return m
                else:
                    j+=1
            else:
                j +=1          
    
    return m                  
    
# Funções adicionais

def jogo_ganho(m):
    """
    é uma função auxiliar que recebe um campo do jogo das minas e devolve
    True se todas as parcelas sem minas se encontram limpas, ou False caso contrário
    """
    return len(obtem_coordenadas(m,'minadas'))==len(obtem_coordenadas(m,'tapadas')) or len(obtem_coordenadas(m,'minadas'))==len(obtem_coordenadas(m,'tapadas')+obtem_coordenadas(m,'marcadas'))

def turno_jogador(m):
    """
    é uma funçãao auxiliar que recebe um campo de minas e oferece ao jogador
    a opção de escolher uma ação e uma coordenada. A função modifica destrutivamente
    o campo de acordo com ação escolhida, devolvendo False caso o jogador tenha limpo
    uma parcela que continha uma mina, ou True caso contrário.
    """
    acao = input('Escolha uma ação, [L]impar ou [M]arcar:')
    
    if acao != 'M' and acao != 'L':
        return turno_jogador(m)
    else:
        res = ()
        coord = input('Escolha uma coordenada:')
        if acao == 'M':                           
                m['linhas'][str_para_coordenada(coord)[1]][ord(coord[0])] = alterna_bandeira(obtem_parcela(m,str_para_coordenada(coord)))
                return m
        if acao == 'L':
            limpa_campo(m,str_para_coordenada(coord))
            if eh_parcela_minada(obtem_parcela(m,str_para_coordenada(coord))):
                res += (str_para_coordenada(coord),)
                
    return len(res)==0

  
                
        

def minas(c,l,n,d,s):
    """
    é a função principal que permite jogar ao jogo das minas. A função
    recebe uma cadeia de carateres e 4 valores inteiros correspondentes, respetivamente, a:
    última coluna c; última linha l; número de parcelas com minas n; dimensão do gerador
    de números d; e estado inicial ou seed s para a geração de números aleatórios. A função
    devolve True se o jogador conseguir ganhar o jogo, ou False caso contrário.
    """
    if type(c) != str or c < 'A' or c > 'Z' or type(l) != int or len (c) > 1 or l < 0 or l > 99 or type(n) != int or n < 0 or type(d) != int or (d != 32 or d != 64) or type(s) != int or s < 0 or s > 2**d or ((l + (ord(c)-ord('A')))<12) or n >=(l+(ord(c)-ord('A'))):
        raise ValueError('minas: argumentos invalidos')
    m = cria_campo(c,l)
    g = cria_gerador(d,s)
    x = input('Escolha uma ação, [L]impar ou [M]arcar:')
    print('[Bandeiras',len(obtem_coordenadas(m,'marcadas')),'/',n,']')
    if x != 'M' and x != 'L':
        x =  input('Escolha uma ação, [L]impar ou [M]arcar:')
    else:
        
        y = input('Escolha uma coordenada:')
        if x == 'M':                           
                m['linhas'][str_para_coordenada(y)[1]][ord(y[0])] = alterna_bandeira(obtem_parcela(m,str_para_coordenada(y)))
                print(campo_para_str(m))
                return m
        if x == 'L':
            coloca_minas(m,str_para_coordenada(y),g,n)
            limpa_campo(m,str_para_coordenada(y))
            if eh_parcela_minada(obtem_parcela(m,str_para_coordenada(y))):
                print(campo_para_str(m))
                print('BOOOOOOOM!!!')
                return jogo_ganho(m)
            else:
                if jogo_ganho(m):
                    print(campo_para_str(m))
                    print('VITORIA!!!')
                    return jogo_ganho(m)
                else:
                    return turno_jogador(m)
