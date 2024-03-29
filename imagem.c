#include "imagem.h"

/****************************************************
Função: criarImagem
Parâmetros: nenhum
Retorno: estrutura Imagem

Descrição: serve para inicializar uma nova imagem, recebendo as entradas necessárias
e retornando os valores atribuídos em uma estrutura do tipo Imagem.
*****************************************************/
Imagem criarImagem(int *imagemAberta, int lar, int alt){
	Imagem imagem;

	/* atribuindo configurações padrão */
	strcpy(imagem.id, "P3");
	imagem.max = 255;

	imagem.lar = lar;
	imagem.alt = alt;
	imagem.numDePixels = imagem.lar * imagem.alt;

	/* alocação dinâmica da matriz pixels e de sua cópia */
	imagem.pixels = (Cor **) malloc(alt * sizeof(Cor *));
	imagem.pixelsCopy = (Cor **) malloc(alt * sizeof(Cor *));
	if (!imagem.pixels || !imagem.pixelsCopy){
		exit(1);
	}

	for (int i = 0; i < alt; i++){
		imagem.pixels[i] = (Cor *) malloc(lar * sizeof(Cor));
		imagem.pixelsCopy[i] = (Cor *) malloc(lar * sizeof(Cor));
		if (!imagem.pixels[i] || !imagem.pixelsCopy[i])
			exit(1);
	}

	/* pintando a imagem de branco */
	for (int y = 0; y < imagem.alt; y++)
		for (int x = 0; x < imagem.lar; x++){
			imagem.pixels[y][x].r = 255;
			imagem.pixels[y][x].g = 255;
			imagem.pixels[y][x].b = 255; 
		}

	/* atribuindo a cor preta ao pincel */
	imagem.cor.r = 0;
	imagem.cor.g = 0;
	imagem.cor.b = 0;

	/* inicia uma estrutura de desenho */
	imagem.desenho = criarDesenho();

	*imagemAberta = 1;

	return imagem;
}

/****************************************************
Função: abrirImagem
Parâmetros: nenhum
Retorno: tipo Imagem

Descrição: lê um arquivo ppm, atribui a uma estrutura Imagem e 
retorna essa estrutura.
*****************************************************/
Imagem abrirImagem(int *imagemAberta, char caminho[100]){
	Imagem imagem;
	imagem.arquivo = fopen(caminho, "r");

	if (imagem.arquivo == NULL){
		*imagemAberta = 0;
		return imagem;
	}

	/* leitura de conteudo */
	fscanf(imagem.arquivo, "%s\n", imagem.id);
	fscanf(imagem.arquivo, "%d %d", &imagem.lar, &imagem.alt);
	imagem.numDePixels = imagem.lar * imagem.alt;
	fscanf(imagem.arquivo, "%d", &imagem.max);

	/* alocação dinâmica da matriz pixels e de sua cópia */
	imagem.pixels = (Cor **) malloc(imagem.alt * sizeof(Cor *));
	imagem.pixelsCopy = (Cor **) malloc(imagem.alt * sizeof(Cor *));
	if (!imagem.pixels || !imagem.pixelsCopy){
		exit(1);
	}

	for (int i = 0; i < imagem.alt; i++){
		imagem.pixels[i] = (Cor *) malloc(imagem.lar * sizeof(Cor));
		imagem.pixelsCopy[i] = (Cor *) malloc(imagem.lar * sizeof(Cor));
		if (!imagem.pixels[i] || !imagem.pixelsCopy[i])
			exit(1);
	}

	/* lendo pixels da imagem e atribuindo à matriz */
	for (int i = 0; i < imagem.alt; ++i){
		for (int j = 0; j < imagem.lar; ++j){
			fscanf(imagem.arquivo, "%d %d %d\n",
				&imagem.pixels[i][j].r,
				&imagem.pixels[i][j].g,
				&imagem.pixels[i][j].b);
		}
	}

	/* atribuindo a cor preta ao pincel */
	imagem.cor.r = 0;
	imagem.cor.g = 0;
	imagem.cor.b = 0;

	/* inicia uma estrutura de desenho */
	imagem.desenho = criarDesenho();

	fclose(imagem.arquivo);

	*imagemAberta = 1;

	return imagem;
}

/****************************************************
Função: salvarImagem
Parâmetros: ponteiro tipo Imagem
Retorno: nenhum

Descrição: cria um novo arquivo ppm, cria uma cópia da matriz de pixels
para que os desenhos sejam inseridos sem comprometer a imagem original,
e escreve todas os valores da matriz no arquivo criado.
*****************************************************/
void salvarImagem(Imagem *imagem){
	/* criando cópia da matriz de pixels */
	for (int y = 0; y < imagem->alt; y++){
		for (int x = 0; x < imagem->lar; x++){
			imagem->pixelsCopy[y][x] = imagem->pixels[y][x];
		}
	}

	/* criando novo arquivo */
	char path[100] = {"./galeria/"};
	strcat(path, imagem->nomeDoArquivo);
	imagem->arquivo = fopen(path, "w");
	//printf("arquivo: %s\n", imagem->nomeDoArquivo);

	/* inserindo desenhos na imagem */
	inserirDesenhos(imagem);

	/* inserindo informações de id, dimensões e valor de cor máximo */
	fprintf(imagem->arquivo, "%s\n", imagem->id);
	fprintf(imagem->arquivo, "%d %d\n", imagem->lar, imagem->alt);
	fprintf(imagem->arquivo, "%d\n", imagem->max);

	/* escrevendo matriz de pixels no arquivo */
	for (int y = 0; y < imagem->alt; y++){
		for (int x = 0; x < imagem->lar; x++){
			fprintf(imagem->arquivo, "%d ", imagem->pixelsCopy[y][x].r);
			fprintf(imagem->arquivo, "%d ", imagem->pixelsCopy[y][x].g);
			fprintf(imagem->arquivo, "%d\n", imagem->pixelsCopy[y][x].b);
		}
	}

	fclose(imagem->arquivo);
}

/****************************************************
Função: checaImagem
Parâmetros: inteiro imagemAberta, inteiro temArquivo, arquivo tipo FILE
Retorno: inteiro

Descrição: verifica se há uma imagem aberta, se não houver, limpa o
buffer e retorna 0. Se existir uma imagem aberta, será retornado 1.
*****************************************************/
int checaImagem(int imagemAberta, int temArquivo, FILE *arq){
	if (!imagemAberta){
			printf("Erro: imagem nao aberta\n");
			limparBuffer(temArquivo, arq);
			return 0;
		}

	return 1;
}

/****************************************************
Função: limparImagem
Parâmetros: ponteiro tipo Imagem
Retorno: nenhum

Descrição: limpa toda a imagem para uma cor especificada
*****************************************************/
void limparImagem(Imagem *imagem, Cor cor){
	for (int i = 0; i < imagem->alt; ++i)
		for (int j = 0; j < imagem->lar; ++j){
			imagem->pixels[i][j].r = cor.r;
			imagem->pixels[i][j].g = cor.g;
			imagem->pixels[i][j].b = cor.b;
		}

	/* todos os desenhos serão apagados */
	imagem->desenho = criarDesenho();
}

/****************************************************
Função: criarDesenho
Parâmetros: nenhum
Retorno: estrutura Desenho

Descrição: responsável por criar uma estrutura Desenho e retorná-la
*****************************************************/
Desenho criarDesenho(void){
	Desenho d;

	d.numLinhas = 0;
	d.numPoligonos = 0;
	d.numCirculos = 0;
	d.numPreencher = 0;
	d.numOrdem = 0;

	return d;
};

/****************************************************
Função: listarDesenhos
Parâmetros: tipo Desenho
Retorno: nenhum

Descrição: imprime na tela todos os desenhos feitos na imagem
*****************************************************/
void listarDesenhos(Desenho d, int temArquivo){
	if (temArquivo)
			printf("\n"); // quebra de linha final

	int printed; //counters
	int lin = 0;
	int pol = 0;
	int cir = 0;
	int pre = 0;

	printf("--------------------------------------------\n");
	for (printed = 0; printed < d.numOrdem;){
		switch(d.ordem[printed]){
			case 1:
				printf("%02d - linha %d - p1 (%d,%d), p2 (%d,%d)\n", ++printed, lin + 1, 
					d.linhas[lin].inicio.x,
					d.linhas[lin].inicio.y,
					d.linhas[lin].fim.x,
					d.linhas[lin].fim.y);
				lin++;
				break;

			case 2:
				printf("%02d- poligono %d -", printed++, pol + 1);
				for (int j = 0; j < d.poligonos[pol].numFaces; ++j){
					printf(" p%d (%d,%d)", j+1,
						d.poligonos[pol].pontos[j].x,
						d.poligonos[pol].pontos[j].y);
				}
				pol++;
				printf("\n");
				break;

			case 3:
				printf("%02d - circulo %d - centro: (%d,%d) raio: %d\n", ++printed, cir + 1,
					d.circulos[cir].centro.x,
					d.circulos[cir].centro.y,
					d.circulos[cir].raio);
				cir++;
				break;

			case 4:
				printf("%02d - preencher %d - ponto: (%d,%d) cor (%d, %d, %d)\n", ++printed, pre + 1,
					d.preencher[pre].ponto.x,
					d.preencher[pre].ponto.y,
					d.preencher[pre].novaCor.r,
					d.preencher[pre].novaCor.g,
					d.preencher[pre].novaCor.b);
				pre++;
				break;

		}
	}

	if (!d.numOrdem){
		printf("A imagem atual nao possui desenhos\n\n");
		return;
	}
	printf("--------------------------------------------\n\n");
}

/****************************************************
Função: inserirDesenhos
Parâmetros: nenhum
Retorno: ponteiro tipo Imagem

Descrição: aplica todos os desenhos na matriz de pixel, essa função é
chamada apenas na etapa de salvamento do arquivo ppm
*****************************************************/
void inserirDesenhos(Imagem *imagem){
	int numDesenhos = imagem->desenho.numOrdem;

	int lin = 0;
	int cir = 0;
	int pol = 0;
	int pre = 0;

	for (int i = 0; i < numDesenhos; ++i){
		int d = imagem->desenho.ordem[i];

		if (d == 1){
			inserirLinha(imagem->desenho.linhas[lin++], imagem);
		}

		else if (d == 2){
			inserirPoligono(imagem->desenho.poligonos[cir++], imagem);
		}

		else if (d == 3){
			inserirCirculo(imagem->desenho.circulos[pol++], imagem);
		}

		else if (d == 4){
			Preencher p = imagem->desenho.preencher[pre++];
			p.cor = imagem->pixelsCopy[p.ponto.y][p.ponto.x]; // definindo cor do pixel inicial

			/* caso a cor a ser pintada seja igual a atual*/
			if (compararCor(p.cor, p.novaCor)){
				p.novaCor.r++; // alterando a nova cor
				inserirPreenchimento(p.ponto.x, p.ponto.y, p, imagem);
				p.novaCor.r--; // revertendo a cor
			}

			inserirPreenchimento(p.ponto.x, p.ponto.y, p, imagem);
		}
	}
}