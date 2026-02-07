# IAC 2023/2024 k-means
# 
# Grupo: 14
# Campus: Alameda
#
# Autores:
# 51775, João Carvalho
# 106893, Miguel Blanco
# 107032, Rodrigo Santos
#
# Tecnico/ULisboa


# ALGUMA INFORMACAO ADICIONAL PARA CADA GRUPO:
# - A "LED matrix" deve ter um tamanho de 32 x 32
# - O input e' definido na seccao .data. 
# - Abaixo propomos alguns inputs possiveis. Para usar um dos inputs propostos, basta descomentar 
#   esse e comentar os restantes.
# - Encorajamos cada grupo a inventar e experimentar outros inputs.
# - Os vetores points e centroids estao na forma x0, y0, x1, y1, ...


# Variaveis em memoria
.data

# Input A - linha inclinada
#n_points:    .word 9
#points:      .word 0,0, 1,1, 2,2, 3,3, 4,4, 5,5, 6,6, 7,7 8,8

# Input B - Cruz
#n_points:    .word 5
#points:     .word 4,2, 5,1, 5,2, 5,3 6,2

# Input C
#n_points:    .word 23
#points: .word 0,0, 0,1, 0,2, 1,0, 1,1, 1,2, 1,3, 2,0, 2,1, 5,3, 6,2, 6,3, 6,4, 7,2, 7,3, 6,8, 6,9, 7,8, 8,7, 8,8, 8,9, 9,7, 9,8

# Input D
n_points:    .word 30
points:      .word 16, 1, 17, 2, 18, 6, 20, 3, 21, 1, 17, 4, 21, 7, 16, 4, 21, 6, 19, 6, 4, 24, 6, 24, 8, 23, 6, 26, 6, 26, 6, 23, 8, 25, 7, 26, 7, 20, 4, 21, 4, 10, 2, 10, 3, 11, 2, 12, 4, 13, 4, 9, 4, 9, 3, 8, 0, 10, 4, 10

# Input E
#n_points:     .word 9
#points:       .word 0, 0, 1, 0, 0, 1, 31, 31, 30, 31, 31, 30, 0, 31, 1, 31, 0, 30

# Valores de centroids e k a usar na 1a parte do projeto:
#centroids:   .word 0,0
#k:           .word 1
# Valores de centroids, k e L a usar na 2a parte do prejeto:
centroids:   .word 0,0, 10,0, 0,10
k:           .word 3
L:           .word 10

# Definicoes de cores a usar no projeto 
colors:      .word 0xff0000, 0x00ff00, 0x0000ff  # Cores dos pontos do cluster 0, 1, 2, etc.
.equ         black      0
.equ         white      0xffffff

# Abaixo devem ser declarados o vetor clusters (2a parte) e outras estruturas de dados
# que o grupo considere necessarias para a solucao:
.equ         max_distance 64
# OPTIMIZATION: nada precisa de ser guardado na memória de início, uma vez que tudo pode ou ser guardado na stack,
#               ou pode advir de um retorno de uma função. Isto não prejudica a performance, uma vez que todos os
#               stack pushes/pops acontecem uma vez para cada centroid, introduzindo pouca complexidade algorítmica

# Codigo
.text
    # Chama funcao principal da 1a parte do projeto
    #jal mainSingleCluster
    
    # Descomentar na 2a parte do projeto:
    jal mainKMeans
    
    #Termina o programa (chamando chamada sistema)
    li a7, 10
    ecall


### printPoint
# Pinta o ponto (x,y) na LED matrix com a cor passada por argumento
# Nota: a implementacao desta funcao ja' e' fornecida pelos docentes
# E' uma funcao auxiliar que deve ser chamada pelas funcoes seguintes que pintam a LED matrix.
# Argumentos:
# a0: x
# a1: y
# a2: cor

printPoint:
    li a3, LED_MATRIX_0_HEIGHT
    sub a1, a3, a1
    addi a1, a1, -1
    li a3, LED_MATRIX_0_WIDTH
    mul a3, a3, a1
    add a3, a3, a0
    slli a3, a3, 2
    li a0, LED_MATRIX_0_BASE
    add a3, a3, a0   # addr
    sw a2, 0(a3)
    jr ra
    

### cleanScreen
# Limpa todos os pontos do ecrã
# Argumentos: nenhum
# Retorno: nenhum

cleanScreen:
    addi sp, sp, -16
    sw s0, 0(sp)
    sw s1, 4(sp)
    sw s2, 8(sp)
    sw s3, 12(sp)
    
    la s0, LED_MATRIX_0_BASE
    li s1, LED_MATRIX_0_HEIGHT
    li s2, LED_MATRIX_0_WIDTH
    li s3, white
    
    mul t0, s1, s2 # número de LEDs
    li t1, 0 # contador p/ número de LEDs
    
    loop_leds:
        sw s3, 0(s0) # cor do LED passa a branco
        addi s0, s0, 4 # passo para o próximo LED
        addi t1, t1, 1
        blt t1, t0, loop_leds
    
    lw s0, 0(sp)
    lw s1, 4(sp)
    lw s2, 8(sp)
    lw s3, 12(sp)
    addi sp, sp, 16
    
    jr ra 


### printClusters
# Pinta os agrupamentos na LED matrix com a cor correspondente.
# Argumentos: nenhum
# Retorno: nenhum

printClusters:
    addi sp, sp, -16
    sw s0, 0(sp)
    sw s1, 4(sp)
    sw s2, 8(sp)
    sw ra, 12(sp)
    
    la s0, points
    la s1, colors 
    lw s2, n_points

    li t0, 0 # contador do vetor points
    loop_points:
        lw a0, 0(s0) # coordenada x
        lw a1, 4(s0) # coordenada y
        
        addi sp, sp, -12
        sw t0, 0(sp)
        sw a0, 4(sp)
        sw a1, 8(sp)
        
        jal ra, nearestCluster
        mv a2, a0 # guardo o cluster a que pertence (x, y)
        
        lw t0, 0(sp)
        lw a0, 4(sp)
        lw a1, 8(sp)
        addi sp, sp, 12
        
        slli a2, a2, 2 # indice * 4 = no. de bytes a deslocar o pointer
        add a2, s1, a2 # t3 = endereço da cor
        
        lw a2, 0(a2) # cor do ponto, i.e. cor do cluster a que o ponto pertence
        
        jal ra, printPoint
        
        addi s0, s0, 8 # passo para o próximo ponto
                
        addi t0, t0, 1
        blt t0, s2, loop_points # if (contador < no. de pontos)
    
    lw s0, 0(sp)
    lw s1, 4(sp)
    lw s2, 8(sp)
    lw ra, 12(sp)
    addi sp, sp, 16
    
    jr ra


### printCentroids
# Pinta os centroides na LED matrix
# Nota: deve ser usada a cor preta (black) para todos os centroides
# Argumentos: nenhum
# Retorno: nenhum

printCentroids:
    addi sp, sp, -12
    sw s0, 0(sp)
    sw s1, 4(sp)
    sw ra, 8(sp)
        
    la s0, centroids
    lw s1, k
    li a2, black
    
    li t0, 0 # contador p/ número de centroides
    
    loop_print_centroids:
        lw a0, 0(s0) # coord. x do centroide
        lw a1, 4(s0) # coord. y do centroide
        
        jal ra, printPoint
        
        addi s0, s0, 8 # próximo ponto
        addi t0, t0, 1
        
        blt t0, s1, loop_print_centroids # if (contador < no. centroids)
    
    lw s0, 0(sp)
    lw s1, 4(sp)
    lw ra, 8(sp)
    addi sp, sp, 12
        
    jr ra
    

### calculateCentroids
# Calcula os k centroides, a partir da distribuicao atual de pontos associados a cada agrupamento (cluster)
# Argumentos: nenhum
# Retorno: nenhum

calculateCentroids: 
    addi sp, sp, -16
    sw s0, 0(sp)
    sw s1, 4(sp)
    sw s2, 8(sp)
    sw s3, 12(sp)

    lw s1, n_points
    la s2, centroids # vetor de centroids
    lw s3, k
    
    li t0, 0 # contador para o no. de clusters
    cluster_centroid:
        li t1, 0 # contador para no. de pontos
        
        li t2, 0 # somatorio das coord x dos pontos que pertencem ao cluster 0
        li t3, 0 # somatorio das coord y dos pontos que pertencem ao cluster 0 
        li t4, 0 # no. de pontos que pertencem ao cluster 0
        
        la s0, points # vetor de pontos
        
        sum_coords:
            lw a0, 0(s0) # coord x do ponto
            lw a1, 4(s0) # coord y do ponto
            
            addi sp, sp, -32 # guardo t's e a's 
            sw a0, 0(sp)
            sw a1, 4(sp)
            sw t0, 8(sp)
            sw t1, 12(sp)
            sw t2, 16(sp)
            sw t3, 20(sp)
            sw t4, 24(sp)
            sw ra, 28(sp)
            
            jal ra, nearestCluster
            
            mv t5, a0 # cluster a que pertence o ponto (x, y)
            
            lw a0, 0(sp)
            lw a1, 4(sp)
            lw t0, 8(sp)
            lw t1, 12(sp)
            lw t2, 16(sp)
            lw t3, 20(sp)
            lw t4, 24(sp)
            lw ra, 28(sp)
            addi sp, sp, 32
            
            bne t5, t0, skip_sum # se o ponto não pertence ao cluster, não somo
            
            add t2, t2, a0 # somo x a sum_x, se x pertence ao cluster
            add t3, t3, a1 # faço o mesmo para y
            addi t4, t4, 1 # incremento o no. de pontos que pertence ao cluster
            
            skip_sum:
            addi s0, s0, 8 # avanço para o proximo ponto
            addi t1, t1, 1
            
            blt t1, s1, sum_coords
            
        beqz t4, skip_div # se não foi encontrado nenhum ponto que pertença ao centroid...
                          # ...salta-se a divisão, p/ não dividir por zero
        
        div t2, t2, t4 # média dos x
        div t3, t3, t4 # média dos y
        
        skip_div:
        # guardar os pontos diretamente na memória iria aniquilar centroids antigos, que ainda...
        # ...têm de ser usados em nearestCluster    
        addi sp, sp, -8
        sw t2, 0(sp)
        sw t3, 4(sp)
        
        addi t0, t0, 1
        
        blt t0, s3, cluster_centroid
        
    li t0, 0
    addi t1, s3, -1
    slli t1, t1, 3 # deslocamento em memória para centroids[k - 1] = (k - 1) * 4 * 2
    add s2, s2, t1 # endereço de centroids[k - 1]
    load_results:
        lw t2, 0(sp) # carrego as coords dos centroids e transfiro-os para a memória
        lw t3, 4(sp)
        addi sp, sp, 8
        
        sw t2, 0(s2) # save da coord x
        sw t3, 4(s2) # save da coord y
        
        addi s2, s2, -8
        addi t0, t0, 1
        
        blt t0, s3, load_results
        
    lw s0, 0(sp)
    lw s1, 4(sp)
    lw s2, 8(sp)
    lw s3, 12(sp)
    addi sp, sp, 16
            
    jr ra


### mainSingleCluster
# Funcao principal da 1a parte do projeto.
# Argumentos: nenhum
# Retorno: nenhum

mainSingleCluster:

    #1. Coloca k=1 (caso nao esteja a 1)
    la s0, k
    li t0, 1
    sw t0, 0(s0)
    addi sp, sp, -4
    sw ra, 0(sp)
    
    #2. cleanScreen
    jal ra, cleanScreen
    
    #3. printClusters    
    jal ra, printClusters
    
    #4. calculateCentroids
    jal ra, calculateCentroids
    
    #5. printCentroids ms
    jal ra, printCentroids
    
    #6. Termina
    lw ra,0(sp)
    addi sp, sp, 4
    
    jr ra


### initializeCentroids
# Escolhe, pseudo-aleatoriamente, k pontos para serem os primeiros centroides do algoritmo k-means
# Argumentos: nenhum
# Retorno: nenhum

initializeCentroids:
    addi sp, sp, -16
    sw s0, 0(sp)
    sw s1, 4(sp)
    sw s2, 8(sp)
    sw s3, 12(sp)
        
    lw s0, k
    la s1, centroids
    lw s2, n_points
    la s3, points
    
    li t0, 0 # contador para n0. de pontos a obter
    
    li a7, 30 # ecall obtém um número pseudo-aleatório com base no epoch time do UNIX, que será a seed
    ecall
    
    mv t2, a0 # t2 tem a seed/o ultimo numero gerado

    generate_number:
        li t3, 31 # t3 tem o multiplicador, a
        li t4, 7 # t4 tem o incremento, b
        
        mul t2, t2, t3 # t2 = a * x_n-1
        add t2, t2, t4 # t2 = (a * x_n-1 + b)
        remu t2, t2, s2 # t2 = t2 = (a * x_n-1 + b) mod n_points
        
        li t3, 2
        remu t3, t2, t3 # calcula mod do no. gerado por 2 p/ ver se é par
        
        mv t5, t2
        beqz t3, save_rng_centroid
        addi t5, t5, -1 # se t2 é ímpar, há que subtrair 1 para alinhar com coord. x na leitura do ponto
    
        save_rng_centroid:
            beqz t0, skip_duplicate_check # o primeiro número gerado não é, obviamente, duplicado
            li t6, 0
            check_duplicate:
                lw t3, 0(sp)
                addi sp, sp, 4
                
                beq t3, t5, generate_number 
                addi t6, t6, 1
                blt t6, t0, check_duplicate
            
            skip_duplicate_check:
            addi sp, sp, -4
            sw t5, 0(sp)
                        
            slli t5, t5, 2
            add t5, t5, s3 # t5 tem o address de x
            
            lw t4, 0(t5) # move-se o valor de x para centroids
            sw t4, 0(s1)
            
            lw t4, 4(t5) # e faz-se o mesmo com o valor de y
            sw t4, 4(s1)

        addi s1, s1, 8
        addi t0, t0, 1
    
        blt t0, s0, generate_number
    
    lw s0, 0(sp)
    lw s1, 4(sp)
    lw s2, 8(sp)
    lw s3, 12(sp)
    addi sp, sp, 16
    
    jr ra


### manhattanDistance
# Calcula a distancia de Manhattan entre (x0,y0) e (x1,y1)
# Argumentos:
# a0, a1: x0, y0
# a2, a3: x1, y1
# Retorno:
# a0: distance

manhattanDistance:
    sub t0, a2, a0 # t0 = x1 - x0
    bgtz t0, calc_y
    neg t0, t0
    
    calc_y: # t1 = y1 - y0
    sub t1, a3, a1
    bgtz t1, calc_dist
    neg t1, t1
    
    calc_dist: # dist = |x1 - x0| + |y1 - y0|
    add a0, t0, t1
    
    jr ra
    

### nearestCluster
# Determina o centroide mais perto de um dado ponto (x,y).
# Argumentos:
# a0, a1: (x, y) point
# Retorno:
# a0: cluster index

nearestCluster:
    addi sp, sp, -12
    sw s0, 0(sp)
    sw s1, 4(sp)
    sw s2, 8(sp)
    
    lw s0, k
    la s1, centroids
    li s2, max_distance
    
    li t0, 0 # contador p/ indice do cluster atual
    li t1, 0 # cluster mais proximo
    
    dist_centroid:   
        lw a2, 0(s1) # coord. x do centroide
        lw a3, 4(s1) # coord. y do centroide
        
        addi sp, sp, -20
        sw ra, 0(sp)
        sw t0, 4(sp)
        sw t1, 8(sp)
        sw a0, 12(sp)
        sw a1, 16(sp)
    
        jal ra, manhattanDistance
        
        mv t2, a0
        
        lw ra, 0(sp)
        lw t0, 4(sp)
        lw t1, 8(sp)
        lw a0, 12(sp)
        lw a1, 16(sp)
        addi sp, sp, 20

        bgt t2, s2, skip_update_min

        add s2, t2, x0
        add t1, t0, x0

        skip_update_min:
        addi t0, t0, 1
        addi s1, s1, 8
    
        blt t0, s0, dist_centroid
    
    add a0, t1, x0
    
    lw s0, 0(sp)
    lw s1, 4(sp)
    lw s2, 8(sp)
    
    addi sp, sp, 12
    
    jr ra


### mainKMeans
# Executa o algoritmo *k-means*.
# Argumentos: nenhum
# Retorno: nenhum

mainKMeans:  
    addi sp, sp, -4
    sw ra, 0(sp)
    
    lw s0, k
    la s1, points
    lw s2, n_points
    la s3, centroids
    lw s5, L
    
    jal ra, cleanScreen
    
    jal ra, initializeCentroids # escolhe 3 pontos diferentes para serem centroids
    
    li t0, 0 # contador de iterações do algoritmo
    algorithm_procedure:        
        addi sp, sp, -4
        sw t0, 0(sp)
        
        jal ra, cleanScreen
        jal ra, printClusters # print clusters e centroids da iteração
        jal ra, printCentroids
        
        li t1, 0
        mv t2, s3 # copia do endereço de centroids[0]
        save_prev_centroids:
            lw t4, 0(t2) # carrego e guardo coord x
            addi sp, sp, -4
            sw t4, 0(sp)
                        
            lw t4, 4(t2) # carrego e guardo a coord y
            addi sp, sp, -4
            sw t4, 0(sp)
            
            addi t2, t2, 8
            
            addi t1, t1, 1
            blt t1, s0, save_prev_centroids
        
        jal ra, calculateCentroids # calculo centroids para a próxima iteração
    
        addi t2, s0, -1 # t2 = k - 1
        slli t2, t2, 3
        add t2, s3, t2 # copia do endereço de centroids[k - 1]
        
        li t1, 0 # contador para verificar centroids
        li t6, 0 # indica se são todos iguais ou não    
        find_diff_in_centroids:
            lw t4, 0(sp)  # coord y do centroid anterior
            addi sp, sp, 4
            
            lw t5, 4(t2)  # coord y do centroid mais recente 
                        
            beq t4, t5, skip_first_state_change
            addi t6, t6, 1
            
            skip_first_state_change:
            lw t4, 0(sp)  # coord x do centroid anterior
            addi sp, sp, 4
            
            lw t5, 0(t2)  # coord x do centroid mais recente    
                
            beq t4, t5, skip_second_state_change
            addi t6, t6, 1    
            
            skip_second_state_change:    
            addi t1, t1, 1
            addi t2, t2, -8
            
            blt t1, s0, find_diff_in_centroids
            
            beqz t6, quit
                    
        continue:
        lw t0, 0(sp)
        addi sp, sp, 4
    
        addi t0, t0, 1
        blt t0, s5, algorithm_procedure
        
    quit:
    beq t0, s5, skip_iter_counter_pop
    lw t0, 0(sp)
    addi sp, sp, 4
    
    skip_iter_counter_pop:    
    lw ra, 0(sp)
    addi sp, sp, 4
    
    jr ra
