%57175, Joao Pedro dos Santos Carvalho

:- set_prolog_flag(answer_write_options,[max_depth(0)]). % para listas completas
:- ['dados.pl'], ['keywords.pl']. % ficheiros a importar.

% eventosSemSalas(EventosSemSala) e verdade se EventosSemSala e uma
% lista, ordenada e sem elementos repetidos, de IDs de eventos sem sala


eventosSemSalas(EventosSemSala):-
    findall(ID,evento(ID, _, _, _, semSala),EventosSemSala_Aux),
    sort(EventosSemSala_Aux,EventosSemSala).


% eventosSemSalasDiaSemana(DiaDaSemana, EventosSemSala) e verdade se
% EventosSemSala e uma lista, ordenada e sem elementos repetidos, de IDs
% de eventos sem sala que decorrem em DiaDaSemana(doravante
% segunda-feira, terca-feira, quarta-feira, quinta-feira, sexta-feira,
% sabado)


eventosSemSalasDiaSemana(DiaDaSemana, EventosSemSala):-
    eventosSemSalas(EventosSemSalaTotais),
    findall(ID,horario(ID, DiaDaSemana, _,_, _, _),EventosDiaDaSemana),
    intersection(EventosSemSalaTotais,EventosDiaDaSemana,EventosSemSala).


% eventosSemSalasPeriodo(ListaPeriodos,EventosSemSala) e verdade se
% ListaPeriodos e uma lista de periodos e EventosSemSala e uma
% listaordenada e sem elementos repetidos, de IDs de eventos sem sala
% nos periodos de ListaPeriodos


eventosSemSalasPeriodo(ListaPeriodos, EventosSemSala):-
    eventosSemSalasPeriodo(ListaPeriodos,EventosSemSala_1,[]),
    sort(EventosSemSala_1,EventosSemSala).

eventosSemSalasPeriodo([],EventosSemSala,EventosSemSala):-!.

eventosSemSalasPeriodo([P|R],EventosSemSala,EventosSemSalaAux):-
    (P==p1; P==p2), eventosSemSalas(EventosSemSalaTotais),
    findall(ID,horario(ID,_,_,_,_,P),EventosPeriodo),
    findall(ID,horario(ID,_,_,_,_,p1_2),EventosP12),
    append(EventosPeriodo,EventosP12,EventosSemestre1),
    intersection(EventosSemSalaTotais,EventosSemestre1,EventosSemSalaSemestre1),
    append(EventosSemSalaSemestre1,EventosSemSalaAux,N_EventosSemSalaAux),
    eventosSemSalasPeriodo(R,EventosSemSala,N_EventosSemSalaAux).

eventosSemSalasPeriodo([P|R],EventosSemSala,EventosSemSalaAux):-
    (P==p3; P==p4), eventosSemSalas(EventosSemSalaTotais),
    findall(ID,horario(ID,_,_,_,_,P),EventosPeriodo),
    findall(ID,horario(ID,_,_,_,_,p3_4), EventosP34),
    append(EventosPeriodo,EventosP34,EventosSemestre2),
    intersection(EventosSemSalaTotais,EventosSemestre2,EventosSemSalaSemestre2),
    append(EventosSemSalaSemestre2,EventosSemSalaAux,N_EventosSemSalaAux),
    eventosSemSalasPeriodo(R,EventosSemSala,N_EventosSemSalaAux).


% organizaEventos(ListaEventos, Periodo, EventosNoPeriodo) e verdade se
% EventosNoPeriodo e a lista, ordenada e sem elementos repetidos, de IDs
% dos eventos de ListaEventos que ocorrem no Periodo


organizaEventos(ListaEventos,Per,EventosNoPeriodo):-
    organizaEventos(ListaEventos,Per,EventosNoPeriodo_1,[]),
    sort(EventosNoPeriodo_1,EventosNoPeriodo).

organizaEventos([],_,EventosNoPeriodo_1,EventosNoPeriodo_1):-!.

organizaEventos([P|R],Per,EventosNoPeriodo_1,EventosNoPeriodoAux):-
    horario(P,_,_,_,_,Periodo),
    Per == Periodo,! , append(EventosNoPeriodoAux,[P],N_EventosNoPeriodoAux),
    organizaEventos(R,Per,EventosNoPeriodo_1,N_EventosNoPeriodoAux).

organizaEventos([P|R],Per,EventosNoPeriodo_1,EventosNoPeriodoAux):-
    (Per == p1; Per == p2), horario(P,_,_,_,_,Periodo),
    Per \== Periodo, Periodo == p1_2,!,
    append(EventosNoPeriodoAux,[P],N_EventosNoPeriodoAux),
    organizaEventos(R,Per,EventosNoPeriodo_1,N_EventosNoPeriodoAux).

organizaEventos([P|R],Per,EventosNoPeriodo_1,EventosNoPeriodoAux):-
    (Per == p3; Per == p4), horario(P,_,_,_,_,Periodo),
    Per \== Periodo, Periodo == p3_4,!,
    append(EventosNoPeriodoAux,[P],N_EventosNoPeriodoAux),
    organizaEventos(R,Per,EventosNoPeriodo_1,N_EventosNoPeriodoAux).

organizaEventos([P|R],Per,EventosNoPeriodo_1,EventosNoPeriodoAux):-
    horario(P,_,_,_,_,Periodo), Per \== Periodo,
    organizaEventos(R,Per,EventosNoPeriodo_1,EventosNoPeriodoAux).


% eventosMenoresQue(Duracao, ListaEventosMenoresQue) e verdade se
% ListaEventosMenoresQue e a lista ordenada e sem elementos repetidos
% dos identificadores dos eventos que tem duracao menor ou igual a
% Duracao


eventosMenoresQue(Dur,ListaEventosMenoresQue):-
    findall(ID,(horario(ID,_,_,_,Duracao,_),Duracao =<Dur),ListaEventosMenoresQue_Aux),
    sort(ListaEventosMenoresQue_Aux,ListaEventosMenoresQue).


% eventosMenoresQueBool(ID,Duracao) e verdade se o evento identificado
% por ID tiver duracao igual ou menor a Duracao


eventosMenoresQueBool(ID,Dur):-
    horario(ID,_,_,_,Duracao,_), Duracao =< Dur.


% procuraDisciplinas(Curso, ListaDisciplinas) e verdade se
% ListaDisciplinas e a lista ordenada alfabeticamente do nome das
% disciplinas do curso Curso


procuraDisciplinas(Curso,ListaDisciplinas):-
    findall(NomeDisciplina,(turno(ID,Curso,_,_),evento(ID,NomeDisciplina,_,_,_)),ListaDisciplinas_Aux),
    sort(ListaDisciplinas_Aux,ListaDisciplinas).


% organizaDisciplinas(ListaDisciplinas, Curso, Semestres) e verdade se
% Semestres e uma lista com duas listas. A lista na primeira posicao
% contem as disciplinas de ListaDisciplinas do curso Curso que ocorrem
% no primeiro semestre; idem para a lista na segunda posicao, que contem
% as que ocorrem no segundo semestre. Ambas as listas estao ordenadas
% alfabeticamente e nao tem elementos repetidos. O perdicado falha se
% nao existir no curso Curso uma disciplina de ListaDisciplinas


organizaDisciplinas(ListaDisciplinas,Curso,Semestres):-
    organizaDisciplinas(ListaDisciplinas,Curso,Semestres,[[],[]]).

organizaDisciplinas([],_,Semestres,Semestres):-!.

organizaDisciplinas([P|R],Curso,Semestres,[Semestre1,Semestre2]):-
    evento(ID,P,_,_,_), turno(ID,SiglaCurso,_,_),
    SiglaCurso == Curso, horario(ID,_,_,_,_,Periodo),
    (Periodo==p1; Periodo==p2; Periodo==p1_2),!,
    append(Semestre1,[P],Semestre1_Aux), sort(Semestre1_Aux,N_Semestre1),
    organizaDisciplinas(R,Curso,Semestres,[N_Semestre1,Semestre2]).

organizaDisciplinas([P|R],Curso,Semestres,[Semestre1,Semestre2]):-
    evento(ID,P,_,_,_), turno(ID,SiglaCurso,_,_),
    SiglaCurso == Curso, horario(ID,_,_,_,_,Periodo),
    (Periodo==p3; Periodo==p4; Periodo==p3_4),!,
    append(Semestre2,[P],Semestre2_Aux),
    sort(Semestre2_Aux,N_Semestre2),
    organizaDisciplinas(R,Curso,Semestres,[Semestre1,N_Semestre2]).

organizaDisciplinas([P|_],Curso,_,_):-
    evento(ID,P,_,_,_),
    turno(ID,SiglaCurso,_,_),
    SiglaCurso \== Curso,!,fail.

% horasCurso(Periodo,Curso,Ano,TotalHoras) e verdade se TotalHoras for o
% numero de horas total dos eventos associados ao curso Curso, no ano
% Ano e periodo Periodo


horasCurso(Per,Curso,Ano,TotalHoras):-
    findall(ID,turno(ID,Curso,Ano,_),ListaID_Aux),
    sort(ListaID_Aux,ListaID),
    sumHoras(ListaID,Per,TotalHoras).

sumHoras(ListaID,Per,TotalHoras):-
    sumHoras(ListaID,Per,TotalHoras,0).

sumHoras([],_,TotalHoras,TotalHoras):-!.

sumHoras([P|R],Per,TotalHoras,Total_Aux):-
    (Per == p1; Per == p2),
    horario(P,_,_,_,Duracao,Periodo),
    (Periodo == Per ; Periodo == p1_2),
    N_Total_Aux is Total_Aux + Duracao,
    sumHoras(R,Per,TotalHoras,N_Total_Aux).

sumHoras([P|R],Per,TotalHoras,Total_Aux):-
    (Per == p1; Per == p2),
    horario(P,_,_,_,_,Periodo),
    (Periodo \== Per, Periodo \== p1_2),!,
    sumHoras(R,Per,TotalHoras,Total_Aux).

sumHoras([P|R],Per,TotalHoras,Total_Aux):-
    (Per == p3; Per == p4),
    horario(P,_,_,_,Duracao,Periodo),
    (Periodo == Per ; Periodo == p3_4),!,
    N_Total_Aux is Total_Aux + Duracao,
    sumHoras(R,Per,TotalHoras,N_Total_Aux).

sumHoras([P|R],Per,TotalHoras,Total_Aux):-
    (Per == p3; Per == p4),
    horario(P,_,_,_,_,Periodo),
    (Periodo \== Per, Periodo \== p3_4),
    sumHoras(R,Per,TotalHoras,Total_Aux).


% evolucaoHorasCurso(Curso,Evolucao) e verdade se Evolucao for uma lista
% de tuplos na forma(Ano,Periodo,NumHoras), em que NumHoras e o total de
% horas associadas ao curso Curso, no ano Ano e periodo Periodo.


calculoEvolucaoHorasCurso(Curso,[Periodo,Ano],Evolucao):-
    horasCurso(Periodo,Curso,Ano,TotalHoras),
    Evolucao = (Ano,Periodo,TotalHoras).

evolucaoHorasCurso(Curso,Evolucao):-
    ListaPeriodos = [p1,p2,p3,p4],
    ListaAnos = [1,2,3],
    findall([X,Y],(member(X,ListaPeriodos),member(Y,ListaAnos)),Lista_Pares),
    maplist(calculoEvolucaoHorasCurso(Curso),Lista_Pares,Evolucao_Aux),
    sort(Evolucao_Aux,Evolucao).


% ocupaSlot(HoraInicioDada, HoraFimDada, HoraInicioEvento,
% HoraFimEvento,Horas) e verdade se Horas for o numero de horas
% sobrepostas entre o evento que tem inicio em HoraInivioEvento e o fim
% em HoraFimEvento, e o slot que tem inicio em HoraInicioDada e fim em
% HoraFimDada.


ocupaSlot(HoraInicioDada,HoraFimDada,HoraInicioEvento,HoraFimEvento,_):-
    (HoraFimDada=<HoraInicioEvento;HoraInicioDada>=HoraFimEvento),!,fail.

ocupaSlot(HoraInicioDada,HoraFimDada,HoraInicioEvento,HoraFimEvento,Horas):-
    (HoraInicioDada >= HoraInicioEvento, HoraFimDada=<HoraFimEvento),!,
    Horas is HoraFimDada - HoraInicioDada.

ocupaSlot(HoraInicioDada,HoraFimDada,HoraInicioEvento,HoraFimEvento,Horas):-
    (HoraInicioDada<HoraInicioEvento, HoraFimDada>HoraFimEvento),!,
    Horas is HoraFimEvento - HoraInicioEvento.

ocupaSlot(HoraInicioDada,HoraFimDada,HoraInicioEvento,HoraFimEvento,Horas):-
    ((HoraInicioDada =< HoraInicioEvento, HoraFimDada < HoraFimEvento);
    (HoraInicioDada < HoraInicioEvento, HoraFimDada =< HoraFimEvento)),!,
    Horas is HoraFimDada - HoraInicioEvento.

ocupaSlot(HoraInicioDada,HoraFimDada,HoraInicioEvento,HoraFimEvento,Horas):-
    ((HoraInicioDada > HoraInicioEvento, HoraFimEvento =< HoraFimDada);
    (HoraInicioDada >= HoraInicioEvento, HoraFimEvento < HoraFimDada)),
    Horas is HoraFimEvento - HoraInicioDada.


% numHorasOcupadas(Periodo, TipoSala, DiaSemana, HoraInicio, HoraFim,
% SomaHoras) e verdade se SomaHoras for o numero de horas ocupadas nas
% salas do tipo TipoSala, no intervalo de tempo definido entre
% HoraInicio e HoraFim, no dia da semana DiaSemana, e no periodo
% Periodo.


numHorasOcupadas(Periodo, TipoSala, DiaSemana, HoraInicioEvento, HoraFimEvento, SomaHoras):-
    (Periodo == p1; Periodo == p2),!,
    findall(ID,(horario(ID,DiaSemana,_,_,_,Periodo);horario(ID,DiaSemana,_,_,_,p1_2)),ListaID),
    sumHoras(ListaID,HoraInicioEvento,HoraFimEvento,TipoSala,SomaHoras).

numHorasOcupadas(Periodo, TipoSala, DiaSemana, HoraInicioEvento, HoraFimEvento, SomaHoras):-
    (Periodo == p3; Periodo == p4),!,
    findall(ID,(horario(ID,DiaSemana,_,_,_,Periodo);horario(ID,DiaSemana,_,_,_,p3_4)),ListaID),
    sumHoras(ListaID,HoraInicioEvento,HoraFimEvento,TipoSala,SomaHoras).

sumHoras(ListaID,HoraInicioEvento,HoraFimEvento,TipoSala,SomaHoras):-
    sumHoras(ListaID,HoraInicioEvento,HoraFimEvento,TipoSala,SomaHoras,0).

sumHoras([],_,_,_,SomaHoras,SomaHoras):-!.

sumHoras([P|R],HoraInicioEvento,HoraFimEvento,TipoSala,SomaHoras,Sum_Acc):-
    salas(TipoSala,ListaSalas),
    evento(P,_,_,_,Sala),
    member(Sala,ListaSalas),
    horario(P,_,HoraInicioDada,HoraFimDada,_,_),
    ocupaSlot(HoraInicioDada,HoraFimDada,HoraInicioEvento,HoraFimEvento,Horas),
    N_Sum_Acc is Sum_Acc + Horas,
    sumHoras(R,HoraInicioEvento,HoraFimEvento,TipoSala,SomaHoras,N_Sum_Acc).

sumHoras([P|R],HoraInicioEvento,HoraFimEvento,TipoSala,SomaHoras,Sum_Acc):-
    salas(TipoSala,ListaSalas),
    evento(P,_,_,_,Sala),
    member(Sala,ListaSalas),
    horario(P,_,HoraInicioDada,HoraFimDada,_,_),
    \+ocupaSlot(HoraInicioDada,HoraFimDada,HoraInicioEvento,HoraFimEvento,_),!,
    sumHoras(R,HoraInicioEvento,HoraFimEvento,TipoSala,SomaHoras,Sum_Acc).

sumHoras([P|R],HoraInicioEvento,HoraFimEvento,TipoSala,SomaHoras,Sum_Acc):-
    salas(TipoSala,ListaSalas),
    evento(P,_,_,_,Sala),
    \+member(Sala,ListaSalas),!,
    sumHoras(R,HoraInicioEvento,HoraFimEvento,TipoSala,SomaHoras,Sum_Acc).


% ocupacaoMax(TipoSala,HoraInicio,HoraFim,Max) e verdade se Max for o
% numero de horas possiveis de ser ocupadas por sala do tipo TipoSala,
% no intervalo de tempo definido entre HoraInicio e HoraFim.


ocupacaoMax(TipoSala, HoraInicio, HoraFim, Max) :-
    salas(TipoSala,ListaSalas),
    length(ListaSalas,NumeroSalas),
    Max is ((HoraFim - HoraInicio) * NumeroSalas).


% percentagem(SomaHoras, Max, Percentagem) e verdade se Percentagem for
% a divisao de SomaHoras por Max, multiplicada por 100.


percentagem(SomaHoras, Max, Percentagem) :-
    Percentagem is ((SomaHoras/Max)*100).


% ocupacaoCritica(HoraInicio, HoraFim, Threshold,Resultados) e verdade
% se Resultados for uma lista ordenada de tuplos do tipo
% casosCriticos(DiaSemana,TipoSala,Percentagem) em que DiaSemana,
% TipoSala e Percentagem sao, respetivamente, um dia da semana, um tipo
% de sala e a sua percentagem de ocupacao, no intervalo de tempo entre
% HoraInicio e HoraFim, e supondo que a percentagem de ocupacao relativa
% a esses elementos esta acima de um dado valor critico (Threshold).

empty([]).

calculoOcupacaoCritica(HoraInicio,HoraFim,Threshold,[Periodo,TipoSala,DiaSemana],Resultado):-
    numHorasOcupadas(Periodo,TipoSala,DiaSemana,HoraInicio,HoraFim,SomaHoras),
    ocupacaoMax(TipoSala, HoraInicio,HoraFim,Max),
    percentagem(SomaHoras,Max,Perce),
    Perce>Threshold,
    ceiling(Perce,Percentagem),
    Resultado = casosCriticos(DiaSemana,TipoSala,Percentagem),!.

calculoOcupacaoCritica(HoraInicio,HoraFim,Threshold,[Periodo,TipoSala,DiaSemana],Resultado):-
    numHorasOcupadas(Periodo,TipoSala,DiaSemana,HoraInicio,HoraFim,SomaHoras),
    ocupacaoMax(TipoSala, HoraInicio,HoraFim,Max),
    percentagem(SomaHoras,Max,Perce),
    Perce=<Threshold,
    Resultado=[].

ocupacaoCritica(HoraInicio,HoraFim,Threshold,Resultados):-
    ListaPeriodos=[p1,p2,p3,p4],
    findall(TipoSala,salas(TipoSala,_),ListaTipoSalas),
    ListaDiaSemana=[segunda-feira,terca-feira,quarta-feira,quinta-feira,sexta-feira,sabado],
    findall([X,Y,Z],(member(X,ListaPeriodos),member(Y,ListaTipoSalas),member(Z,ListaDiaSemana)),ListaAux),
    maplist(calculoOcupacaoCritica(HoraInicio,HoraFim,Threshold),ListaAux,Resultados_Aux),
    exclude(empty,Resultados_Aux,Resultados_1),
    sort(Resultados_1,Resultados).


% ocupacaoMesa(ListaPessoas, ListaRestricoes, OcupacaoMesa) e verdade se
% ListaPessoas for a lista com o nome das pessoas a sentar a mesa,
% ListaRestricoes for a lista de restricoes a verificar e OcupacaoMesa
% for uma lista com tres listas, em que a primeira contem as pessoas de
% um lado da mesa, a segunda as pessoas a cabeceira e a terceira as
% pessoas do outro lado da mesa, de modo a que essas pessoas sao
% exatamente as da ListaPessoas e verificam todas as restricoes de
% ListaRestricoes.

mesa([[_,_,_],[_,_],[_,_,_]]).

pessoa(A,[[A,_,_],[_,_],[_,_,_]]).
pessoa(A,[[_,A,_],[_,_],[_,_,_]]).
pessoa(A,[[_,_,A],[_,_],[_,_,_]]).
pessoa(A,[[_,_,_],[A,_],[_,_,_]]).
pessoa(A,[[_,_,_],[_,A],[_,_,_]]).
pessoa(A,[[_,_,_],[_,_],[A,_,_]]).
pessoa(A,[[_,_,_],[_,_],[_,A,_]]).
pessoa(A,[[_,_,_],[_,_],[_,_,A]]).

% Define todas as posições onde duas pessoas (A e B) ESTÃO lado a lado.
lado(A, B, Mesa) :- adjacente(A, B, Mesa).
lado(A, B, Mesa) :- adjacente(B, A, Mesa).

% 1. Ao lado um do outro no Lado 1 (Fila de cima)
adjacente(A, B, [[A, B, _], _, _]).
adjacente(A, B, [[_, A, B], _, _]).

% 2. Ao lado um do outro no Lado 2 (Fila de baixo)
adjacente(A, B, [_, _, [A, B, _]]).
adjacente(A, B, [_, _, [_, A, B]]).

% Negação por falha: A e B não estão ao lado se falharem as regras acima
naoLado(A, B, Mesa) :-
    \+ lado(A, B, Mesa).

cab1(A,[[_,_,_],[A,_],[_,_,_]]).

cab2(A,[[_,_,_],[_,A],[_,_,_]]).

honra(A,B,[[_,_,B],[_,A],[_,_,_]]).
honra(A,B,[[_,_,_],[A,_],[B,_,_]]).

% Define todas as posições onde duas pessoas (A e B) ESTÃO frente a frente.
% 1. Frente a frente nas pontas esquerdas dos lados longos
frente(A, B, [[A, _, _], _, [B, _, _]]).
frente(A, B, [[B, _, _], _, [A, _, _]]).

% 2. Frente a frente no meio dos lados longos
frente(A, B, [[_, A, _], _, [_, B, _]]).
frente(A, B, [[_, B, _], _, [_, A, _]]).

% 3. Frente a frente nas pontas direitas dos lados longos
frente(A, B, [[_, _, A], _, [_, _, B]]).
frente(A, B, [[_, _, B], _, [_, _, A]]).

% 4. Frente a frente nas cabeceiras
frente(A, B, [_, [A, B], _]).
frente(A, B, [_, [B, A], _]).

% Negação por falha: A e B não estão em frente se for impossível provar frente(A, B).
naoFrente(A, B, Mesa) :-
    \+ frente(A, B, Mesa).


chamaPessoas([],_):-!.
chamaPessoas([P|R],OcupacaoMesa):-
    pessoa(P,OcupacaoMesa),
    chamaPessoas(R,OcupacaoMesa).

chamaRestricoes([],_):-!.
chamaRestricoes([P|R],OcupacaoMesa):-
    P=.. [T_c|Args],
    append(Args,[OcupacaoMesa],N_Args),
    Lit=.. [T_c|N_Args],
    Lit,
    chamaRestricoes(R,OcupacaoMesa).

ocupacaoMesa(ListaPessoas, ListaRestricoes, OcupacaoMesa):-
    mesa(OcupacaoMesa),
    chamaPessoas(ListaPessoas,OcupacaoMesa),
    chamaRestricoes(ListaRestricoes,OcupacaoMesa).
