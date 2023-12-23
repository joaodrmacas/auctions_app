
# Projeto Rede de Computadores (TCP e UDP)

## Server AS

O auction server é inicializado com o executável "AS" e ao ser aberto tem argumentos opcionais para alterar a PORT e para ativar o modo verboso.
O server inicializa gerando uma base de dados e fazendo fork() para ter um processo à espera de pedidos TCP e outro pedidos UDP. Quando algum dos processos recebe um
pedido de algum utilizador, este começa um processo filho encarregue de gerir este novo pedido, bloqueando os devidos ficheiros e diretoria que vai utilizar. Assim, conseguimos
garantir paralelismo entre processos e permite com que vários utilizadores se conectem ao server ao mesmo tempo. Este processo encarregue do pedido, termina após enviar a 
resposta de volta para o utilizador e dá exit.
Durante o processo de tratar o pedido, o servidor verifica constantemente a informação enviada pelo utilizador e garante que esta está bem formatada e faz as devidas alterações.
Perante um erro interno ou corrupção da base de dados, o servidor é capaz de identificar e garante que envia um feedback para o utilizador.
O servidor quando necessita de ler informações do utilizador, espera durante 10 segundos e após não receber nada, este termina a ligação com o mesmo.
Por fim, o servidor está pronto para ser interrompido a qualquer momento e acaba de forma segura as suas diferentes ligações. 
Ao ser reinicializado, o servidor apaga a sua base de dados e cria uma nova.

## Client User

O utilizador é inicializado criando uma pasta de assets para si mesmo onde vai guardar qualquer imagem enviada pelo server sobre o item que está a ser leiloado. De seguida
o espera por comandos no stdin para comunicar com o servidor.
A aplicação user garante que os comandos inseridos têm boa formatação e evita ao máximo enviar para o servidor qualquer pedido mal formatado.
Este quando necessita de ler informação do servidor, espera 10 segundos e após não receber nada, este termina a ligação com o servidor.
Por fim, o user está pronto para ser interrompido a qualquer momento e acaba de forma segura as suas diferentes ligações. 


## Utilização

O makefile está feito de forma a que o comando 'make' compile o servidor e o cliente, criando os executaveis de cada um,
na diretoria atual.
Para além disso, o makefile tem:
- 'make clean' para limpar qualquer ficheiro de objetos e executaveis.
- 'make client' para compilar e criar o executavel do user
- 'make server' para compilar e criar o executavel do server
- 'make run_client' para correr o cliente sem flags
- 'make run_server' para correr o server sem flags



