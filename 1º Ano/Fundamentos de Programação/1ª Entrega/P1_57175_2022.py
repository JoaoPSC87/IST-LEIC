#Exercicio 1

def limpa_texto(cad):
    """
    Esta função recebe uma cadeia de carateres qualquer e devolve a cadeia de carateres
    limpa que corresponde à remoção de carateres brancos
    """

    res = ''
    res = cad.strip()
    res = res.split()
    res = ' '.join(res)
    return str(res)

def corta_texto(cad,num):
    """
    Esta função recebe uma cadeia de carateres e um inteiro positivo correspondentes a
    um texto limpo e uma largura de coluna respetivamente, e devolve duas subcadeias
    de carateres limpas: a primeira contendo todas as palavras completas desde o início
    da cadeia original (incluindo os espaços separadores entre elas) até um comprimento
    máximo total igual à largura fornecida, e a segunda cadeia contendo o resto do texto de
    entrada.
    """

    cad1 = ''
    cad2 = ''
    i = num
    if num == len(cad):
        return(cad,cad2)
    else:
        while i >= 0:
        
            if i > len(cad):
                cad1 = cad
                i = -1
                return(cad1,cad2)
            elif cad[i-1] == ' ' and cad[i+1] != ' ':
                cad1 = cad[:i-1]
                cad2 = cad[i-1:]
                i = -1
            elif i == 0:
                cad2 = cad
                i = -1   
            elif cad[i-1] == ' ' :
                cad2 = cad[num:]
                cad1 = cad[:num]
                i = -1
            elif cad[i] == ' ' and cad[i-1] != ' ':
                cad1 = cad[:i]
                cad2 = cad[i:]
                i = -1
            else:
                i -= 1
        if cad1 == '':
           raise ValueError('justifica_texto: argumentos invalidos')
        return ((limpa_texto(cad1), limpa_texto(cad2)))


def insere_espacos(cad,num):
    """
    Esta função recebe uma cadeia de carateres e um inteiro positivo correspondentes a um
    texto limpo e uma largura de coluna respetivamente, e no caso da cadeia de entrada
    conter duas ou mais palavras devolve uma cadeia de carateres de comprimento igual
    à largura da coluna formada pela cadeia original com espaços entre palavras. Caso 
    contrário, devolve a cadeia de comprimento igual à largura da coluna
    formada pela cadeia original seguida de espaços.
    """
    res = '' 
    if len(cad) == num:
        res = cad
    elif len(cad) > num:
        raise ValueError('justifica_texto: argumentos invalidos')
    else:
        if ' ' not in cad:
            res = res + (cad +  (' ' * ((num - len(cad)))))
        else:
            espacos_int = ((num - len(cad)) // cad.count(' '))
            espacos_res = ((num - len(cad)) % cad.count(' '))
            res = cad.replace(' ',' '*(espacos_int+1))
            res = res.replace(' '*(espacos_int+1),' '*(espacos_int+2),espacos_res)
    return res

def justifica_texto(cad,num):
    """
    Esta função recebe uma cadeia de carateres não vazia e um inteiro positivo correspondentes
    a um texto qualquer e uma largura de coluna respetivamente, e devolve um tuplo
    de cadeias de carateres justificadas, isto é, de comprimento igual à largura da coluna
    com espaços entre palavras.
    """
    
    res_corta = ()
    res_corta_final = ()
    res_final = ()
    if not isinstance(cad,str) or not isinstance(num,int):
        raise ValueError('justifica_texto: argumentos invalidos')
    cad = limpa_texto(cad)
    if len(cad) == 0:
        raise ValueError('justifica_texto: argumentos invalidos')
    res_corta = corta_texto(cad,num)
    i = 0
    

    
    while i >= 0 :
        if len(res_corta[-1]) > num:
            res_corta = res_corta[:-1] + (corta_texto(res_corta[-1],num))
        else:
            i = -1
           
    if res_corta[-1] == '':
        res_corta_final = res_corta[:-1]
    else:
        res_corta_final = res_corta

    for j in range(len(res_corta_final)-1):
        if len(res_corta_final[j]) < num:
            res_final += (insere_espacos(res_corta_final[j],num),)
            
        else:
            res_final += (res_corta_final[j],)
   
    res_final += (res_corta_final[-1] + (' '*(num-len(res_corta_final[-1]))),)
    
    return res_final

#Exercicio 2

def calcula_quocientes(dct,num):
    """
    Esta função recebe um dicionário com os votos apurados num círculo (com pelo menos
    um partido) e um inteiro positivo representando o número de deputados; e devolve o
    dicionário com as mesmas chaves do dicionário argumento (correspondente a partidos)
    contendo a lista (de comprimento igual ao número de deputados) com os quocientes
    calculados com o método de Hondt ordenados em ordem decrescente.
    """
    res = {}
    
    def f_aux(votos):
        i = 1
        res_votos = []
        while i <= num:
           res_votos = res_votos + [votos/i]
           i = i + 1
        return res_votos

    for partidos in dct:
        res[partidos] =  f_aux(dct[partidos])            
    return res

def atribui_mandatos(dct,num):
    """
    Esta função recebe um dicionário com os votos apurados num círculo e um inteiro representando
    o número de deputados, e devolve a lista ordenada de tamanho igual ao número de deputados
    contendo as cadeias de carateres dos partidos que obtiveram cada mandato, isto é, a primeira
    posição da lista corresponde ao nome do partido que obteve o primeiro deputado, a segunda ao 
    partido que obteve o segundo deputado, etc. No caso de existirem dois ou mais partidos com igual 
    quociente, os mandatos são distribuídos por ordem ascendente às listas menos votadas.
    """
   
    res = []
    i = 0
    
    def f_aux(votos):   #esta função auxiliar devolve os quocientesdo metodo de Hondt para o circulo eleitoral recebido
        res_aux = []
        i = 0
        partidos = list(calcula_quocientes(dct,num))
        votos = list(calcula_quocientes(dct,num).values())
        while i < len(partidos):
            res_aux += [[partidos[i],votos[i]]]
            i += 1
        
        res_aux.sort( key = lambda x:x[1], reverse = True )
        return res_aux
    
    def desempata(quocientes_atuais,res_provisorio):    #esta função resolve empates entre 2 ou mais partidos com o mesmo numero de votos, atribuindo o mandato para o partido com menos deputados eleitos
        res = []
        j = 0
        while j < (len(quocientes_atuais)-1):
            if quocientes_atuais[j][1][0] != quocientes_atuais[j+1][1][0]:
                res_provisorio.append(quocientes_atuais[j][0])
                del quocientes_atuais[j][1][0]
                j = len(quocientes_atuais)
            elif quocientes_atuais[j][1][0] == quocientes_atuais[-1][1][0] and res_provisorio.count(quocientes_atuais[j][0]) != res_provisorio.count(quocientes_atuais[-1][0]):
                    if res_provisorio.count(quocientes_atuais[-2][0]) > res_provisorio.count(quocientes_atuais[-1][0]):
                        res_provisorio.append(quocientes_atuais[-1][0])
                        del quocientes_atuais[-1][1][0]
                        j = len(quocientes_atuais)
                        quocientes_atuais.sort(key = lambda x:x[1], reverse = True)
                    else:
                        res_provisorio.append(quocientes_atuais[-2][0])
                        del quocientes_atuais[-2][1][0]
                        j = len(quocientes_atuais)
                        quocientes_atuais.sort(key = lambda x:x[1], reverse = True)
            elif quocientes_atuais[j][1][0] == quocientes_atuais[j+2][1][0] and res_provisorio.count(quocientes_atuais[j][0]) != res_provisorio.count(quocientes_atuais[j+2][0]):
                j += 1 
            else:
                if res_provisorio.count(quocientes_atuais[j][0]) == res_provisorio.count(quocientes_atuais[j+1][0]):
                    j += 1
                elif res_provisorio.count(quocientes_atuais[j][0]) > res_provisorio.count(quocientes_atuais[j+1][0]):
                    res_provisorio.append(quocientes_atuais[j+1][0])
                    del quocientes_atuais[j+1][1][0]
                    j = len(quocientes_atuais)
                    quocientes_atuais.sort(key = lambda x:x[1], reverse = True)
                elif res_provisorio.count(quocientes_atuais[j][0]) < res_provisorio.count(quocientes_atuais[j+1][0]):
                    res_provisorio.append(quocientes_atuais[j][0])
                    del quocientes_atuais[j][1][0]
                    j = len(quocientes_atuais)
                    quocientes_atuais.sort(key = lambda x:x[1], reverse = True)  
        res.append(quocientes_atuais)        
        res.append(res_provisorio)
        return res

   
    quocientes = f_aux(dct)
    res_desempata = []
    
    while i < num:

        if len(quocientes) == 1:
            res.append(quocientes[0][0])
            del quocientes[0][1][0]
            quocientes.sort(key = lambda x:x[1], reverse = True)
            i += 1

        elif quocientes[0][1][0] != quocientes[1][1][0]:
            res.append(quocientes[0][0])
            del quocientes[0][1][0]
            quocientes.sort(key = lambda x:x[1], reverse = True)
            i += 1                  
        
        else:
            if len(quocientes) != 2:
                res_desempata = desempata(quocientes,res)
                res = res_desempata[1]
                quocientes = res_desempata[0]
                
            else:
                if res.count(quocientes[0][0]) > res.count(quocientes[1][0]):
                    res.append(quocientes[1][0])
                    del quocientes[1][1][0]
                else:
                    res.append(quocientes[0][0])
                    del quocientes[0][1][0]
                quocientes.sort(key = lambda x:x[1], reverse = True)
            i += 1    
    return res

def obtem_partidos(dct):
    """
    Esta funçãao recebe um dicionário com a informação sobre as eleições num território
    com vários círculos eleitorais e devolve a lista por ordem alfabética com
    o nome de todos os partidos que participaram nas eleições.
    """
    res_aux = []
    for territorio in dct:
        res_aux += dct[territorio]['votos']
    res_aux.sort()
    res = []
    for j in range((len(res_aux))):
        if j == (len(res_aux)-1):
            if res_aux[j-1] != res_aux[j]:
                res.append(res_aux[j])
        else:
            if res_aux[j] != res_aux[j-1]:
                res.append(res_aux[j])
    return res

def obtem_resultado_eleicoes(dct):
    """
    Esta função recebe um dicionário com a informação sobre as eleições num território com
    vários círculos eleitorais como descrito, e devolve a lista ordenada de comprimento igual
    ao número total de partidos com os resultados das eleições. Cada elemento da lista é
    um tuplo de tamanho 3 contendo o nome de um partido, o número total de deputados
    obtidos e o número total de votos obtidos. A lista está ordenada por ordem descendente de
    acordo ao número de deputados obtidos e, em caso de empate, de acordo ao
    número de votos.
    """
        
    if type(dct) != dict or len(dct) < 1:
        raise ValueError('obtem_resultado_eleicoes: argumento invalido')
    
    for regiao in dct:
        if type(regiao) != str or type(dct[regiao]) != dict  or 'deputados' not in dct[regiao] or 'votos' not in dct[regiao] or  type(dct[regiao]['votos']) != dict or len(dct[regiao]['votos']) <= 1 or len(dct[regiao]) > 2 or type(dct[regiao]['deputados']) != int or dct[regiao]['deputados'] < 1  :
           raise ValueError('obtem_resultado_eleicoes: argumento invalido') 
        for partidos in dct[regiao]['votos']:
            if type(partidos) != str or  type(dct[regiao]['votos'][partidos]) != int or dct[regiao]['votos'][partidos] <= 0 :
              raise ValueError('obtem_resultado_eleicoes: argumento invalido')

    def f_aux(dct):
        res_aux = []
        for regiao in dct:
            res_aux +=  atribui_mandatos((dct[regiao]['votos']) ,((dct[regiao]['deputados'])))
        return res_aux
    
    res = []
    votos_total = {}
    deputados_total = ()
    partidos = obtem_partidos(dct)
    
    for regiao in dct:
        for partidos in dct[regiao]['votos']:
            if partidos in votos_total :
                votos_total[partidos] += dct[regiao]['votos'][partidos]
            else:
                votos_total[partidos] = dct[regiao]['votos'][partidos]
    
    partidos = obtem_partidos(dct)

    for i in partidos:
        deputados_total += ((i, f_aux(dct).count(i),votos_total[i],),)   
    
    res = list(deputados_total)
    res.sort(key = lambda x:x[1], reverse = True)
    res.sort(key = lambda x:x[2], reverse = True)
    return res


#Exercicio 3

def produto_interno(t1,t2):
    """
    Esta função recebe dois tuplos de números (inteiros ou reais) com a mesma dimensão
    representando dois vetores e retorna o resultado do produto interno desses dois vetores.
    """
    res = 0
    for i in range(len(t1)):
        res += t1[i] * t2[i]
    return float(res)

def verifica_convergencia(t1,t2,t3,num):
    """
    Esta função recebe três tuplos de igual dimensão e um valor real positivo. O primeiro
    tuplo é constituído por um conjunto de tuplos cada um representando uma linha da
    matriz quadrada A, e os outros dois tuplos de entrada contêm valores representando
    respetivamente o vetor de constantes e a solução atual. O valor real de entrada
    indica a precisão pretendida. A função retorna True caso o valor absoluto do
    erro de todas as equações seja inferior à precisão e False caso contrário.
    """
      
    res = False
    res_aux = 0
    for i in range(len(t1)):
         res_aux += (produto_interno(t1[i],t3))
         if abs(res_aux-t2[i]) < num:
                res = True
                res_aux = 0
         else:
                res = False
    return res

def retira_zeros_diagonal(t1,t2):
    """
    Esta função recebe um tuplo de tuplos, representando a matriz de entrada
    e um tuplo de números, representando o vetor das constantes.
    A função retorna uma nova matriz com as mesmas linhas que a de
    entrada, mas com estas reordenadas de forma a não existirem valores 0 na diagonal. O
    segundo parâmetro de saída é também o vetor de entrada com a mesma reordenação de
    linhas que a aplicada à matriz.
    """
    for i in range(len(t2)-1):
        if t1[i] == t1[i+1] and t2[i] != t2[i+1]:
            raise ValueError('resolve_sistema: matriz nao diagonal dominante')
    t1_s = list(t1)
    t2_s = list(t2)
    res = ()
    for i in range(len(t1_s)):
        for j in range(len(t1_s[i])):
            if t1_s[i][j] != 0 and t1_s[j][j] == 0 and t1_s[i][j] != 0:
                t1_s[i],t1_s[j] = t1_s[j],t1_s[i]
                t2_s[i],t2_s[j] = t2_s[j],t2_s[i]
            
    res += (tuple(t1_s),tuple(t2_s))
    return res

def eh_diagonal_dominante(t1):
    """
    Esta função recebe um tuplo de tuplos representando uma matriz quadrada.
    Retorna True caso seja uma matriz diagonalmente dominante e False caso contrário.
    """
    res = False
    i = 0
    j = 0
    soma_el = 0
     
    while i < len(t1):
        while j < len(t1[i]):
            if i == j:
                j +=1
            else:
               soma_el += abs(t1[i][j])
               j += 1

        if t1[i][i] == soma_el == 0:
            raise ValueError('resolve_sistema: matriz nao diagonal dominante')

        elif abs(t1[i][i]) < abs(soma_el):
            res = False
            i = len(t1)
                  
        else:
            res = True
            soma_el = 0
            i += 1
        j = 0
    return res

def resolve_sistema(t1,t2,num):
    """
    Esta função recebe um tuplo de tuplos representando uma matriz quadrada correspondente
    aos coeficientes das equações do sistema, um tuplo de números representando o vetor das
    constantes, e um valor real positivo correspondente à precisão pretendida para a solução.
    Retorna um tuplo que é a solução do sistema de equações de entrada aplicando o método de Jacobi. 
    """
    if not isinstance(t1,tuple) or not isinstance(t2,tuple) or not isinstance(num,float) or num < 0:
        raise ValueError('resolve_sistema: argumentos invalidos')
    if len(t1) != len(t2):
        raise ValueError('resolve_sistema: argumentos invalidos')
    for i in range(len(t1)):
        if type(t1[i]) != tuple or len(t1[i]) != len(t2): 
            raise ValueError('resolve_sistema: argumentos invalidos')
        for j in range(len(t1[i])):
            if type(t1[i][j]) != int and type(t1[i][j]) != float:
                raise ValueError('resolve_sistema: argumentos invalidos')
    for i in range(len(t2)):
        if isinstance(t2[i],str):
            raise ValueError('resolve_sistema: argumentos invalidos')
    
    res = False
    res_aux = retira_zeros_diagonal(t1,t2)
    t1s = res_aux[0]
    t2s = res_aux[1]
    
                
    if not eh_diagonal_dominante(t1s):
        raise ValueError('resolve_sistema: matriz nao diagonal dominante')
    
    
    x_est = () + (0,) * len(t1s)
    x_est_2 = ()
    for i in range(len(t2s)):
        x_est_2 += ((x_est[i] + ((t2s[i]-(produto_interno(t1s[i],x_est)))/t1s[i][i])),)
    
    while res != True:
        res = verifica_convergencia(t1s,t2s,x_est_2,num)
        if res != True:
            x_est_2, x_est = (), x_est_2
            for i in range(len(t2s)):
                x_est_2 += ((x_est[i] + ((t2s[i]-(produto_interno(t1s[i],x_est)))/t1s[i][i])),)
                
        
    return x_est_2
