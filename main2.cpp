#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <cstdlib>
#include <ctime>
#include <math.h>
#include <string>

const double SCREEN_WIDTH = 800;
const double SCREEN_HEIGHT = 800;
int step = 0;
double sland = 0;
bool checkCollision( SDL_Rect a, SDL_Rect b );
bool thua = false;
bool start = false;
bool passed = false;
double Score = 0;

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;

class LTexture
{
    private:
		SDL_Texture* mTexture;
	public:
		LTexture() {
            mTexture = NULL;
		}

		~LTexture() {
            free();
		}

		bool loadFromFile( std::string path )
		{
            free();
            SDL_Texture* newTexture = NULL;
            SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
            if( loadedSurface == NULL )
            {
                printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
            }
            else
            {
                newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
                if( newTexture == NULL )
                {
                    printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
                }
                else
                SDL_FreeSurface( loadedSurface );
            }
            mTexture = newTexture;
            return mTexture != NULL;
        }

		void free() {
		    if( mTexture != NULL )
            {
                SDL_DestroyTexture( mTexture );
                mTexture = NULL;
            }
		}

		void render( double x, double y, double w, double h ) {
            SDL_Rect renderQuad = { x, y, w, h };
            SDL_RenderCopy( gRenderer, mTexture, NULL, &renderQuad );
		}
};

LTexture bird1Texture;
LTexture bird2Texture;
LTexture bird3Texture;
LTexture background;
LTexture land;
LTexture uppipe;
LTexture lowpipe;
LTexture gameOver;
LTexture taptap;
LTexture number[9];

class Bird
{
    private:
        double w = 55;
        double h = 38.5;
        double x = (SCREEN_WIDTH - w)/4;
        double y = (SCREEN_HEIGHT*4/5 - h)/2;
        double speed = 0;
        double g = 0.0002;

    public:

        SDL_Rect chim;
        Bird(){
            chim.x = x;
            chim.h = h;
            chim.w = w;
        }

        void update() {
            if (y + h <= SCREEN_HEIGHT*4/5+3 && start)
            {
                y += speed + 0.5*g;
                chim.y = y;
                speed += g;
            }
        }
        void jump(SDL_Event& e) {
            if (e.type == SDL_MOUSEBUTTONDOWN && !thua) {
                speed = -0.15;
            }
        }
        void render() {
            update();
            if (y + h <= SCREEN_HEIGHT*4/5 +3) {
                if (step%480 >=0 && step%480 <= 119) {
                    bird1Texture.render(x, y, w, h);
                }
                else if (step%480 >= 120 && step%480 <= 239) {
                    bird2Texture.render(x, y, w, h);
                }
                else if (step%480 >= 240 && step%480 <= 359) {
                    bird3Texture.render(x, y, w, h);
                }
                else if (step%480 >= 360 && step%480 <= 479) {
                    bird2Texture.render(x, y, w, h);
                }
                step++;
                if (step > 480) step = 0;
           }
            else thua = true;
        }
};

Bird bird;

class Pipe {
    private:
        double x = 400;
        double y = 300;
        double h = 574;
        double w = 100;
        double b[3];
        double d = 300;
        double ls[3][2];
        double wait = 700;
        double speed = 0.05;
    public:

        SDL_Rect cot;
        SDL_Rect cot2;
        SDL_Rect score;

        Pipe(){
            for (int i = 0; i<3; i++) {
                ls[i][0] = (i+1)*d + 300;
                ls[i][1] = (rand() % 19 + 0) * 25;
                b[i] = (rand() % 11 + 6) * 25;
            }
            cot.w = w;
            cot.h = h;
            cot2.w = w;
            cot2.h = h;
            score.w = 1;
        }

        void render() {
            if (!thua && start) {
                for (int i = 0; i < 3; i++) {
                    if (ls[i][0] + w <= (SCREEN_WIDTH - 55)/4) {
                        cot.y = ls[(i+1)%3][1] - h;
                        cot.x = ls[(i+1)%3][0];
                        cot2.x = ls[(i+1)%3][0];
                        cot2.y = ls[(i+1)%3][1] + b[(i+1)%3];
                    }

                    if (ls[i][0] + w/2 <= (SCREEN_WIDTH - 55)/4 ) {
                        score.x = ls[(i+1)%3][0];
                        score.y = ls[(i+1)%3][1];
                        score.h = b[(i+1)%3];
                    }

                    if (ls[i][0] <= (SCREEN_WIDTH - 55)/4 && passed) {
                        Score +=0.055;
                        passed = false;
                    }

                    if (ls[i][0] <= -100) {
                        ls[i][0] = SCREEN_WIDTH;
                        ls[i][1] = (rand() % 19 + 0) * 25;
                        b[i] = (rand() % 11 + 6) * 25;
                    }
                }

                wait -= speed;
                if (wait < 0) {
                    uppipe.render(ls[0][0], ls[0][1]-h, w, h);
                    lowpipe.render(ls[0][0], ls[0][1]+b[0], w, h);
                }

                uppipe.render(ls[1][0], ls[1][1]-h, w, h);
                lowpipe.render(ls[1][0], ls[1][1]+b[1], w, h);

                uppipe.render(ls[2][0], ls[2][1]-h, w, h);
                lowpipe.render(ls[2][0], ls[2][1]+b[2], w, h);

                ls[0][0] -= speed;
                ls[1][0] -= speed;
                ls[2][0] -= speed;
            }
        }
};

Pipe pipe;


void moveland() {
    if (!thua) {
        sland -= 0.075;
    }
        land.render(sland, SCREEN_HEIGHT*4/5, SCREEN_WIDTH*2, SCREEN_HEIGHT/5);
        if (sland<-800) sland=0;
}

bool init()
{
	bool success = true;

	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		{
			printf( "Warning: Linear texture filtering not enabled!" );
		}
		gWindow = SDL_CreateWindow( "Flappy Bird", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
		    gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED );
			if( gRenderer == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = false;
				}
			}
		}
	}

	return success;
}

bool loadMedia()
{
	bool success = true;

    if( !bird1Texture.loadFromFile( "image/bird1.png" ) )
	{
		printf( "Failed to load image!\n" );
		success = false;
	}

	if( !bird2Texture.loadFromFile( "image/bird2.png" ) )
	{
		printf( "Failed to load image!\n" );
		success = false;
	}

	if( !bird3Texture.loadFromFile( "image/bird3.png" ) )
	{
		printf( "Failed to load image!\n" );
		success = false;
	}

	if( !background.loadFromFile( "image/background.png" ) )
	{
		printf( "Failed to load image!\n" );
		success = false;
	}

	if( !land.loadFromFile( "image/land.png" ) )
	{
		printf( "Failed to load image!\n" );
		success = false;
	}

	if( !uppipe.loadFromFile( "image/uppipe.png" ) )
	{
		printf( "Failed to load image!\n" );
		success = false;
	}

	if( !lowpipe.loadFromFile( "image/lowpipe.png" ) )
	{
		printf( "Failed to load image!\n" );
		success = false;
	}

	if( !gameOver.loadFromFile( "image/gameover.png" ) )
	{
		printf( "Failed to load image!\n" );
		success = false;
	}

	if( !taptap.loadFromFile( "image/flappy-tap.png" ) )
	{
		printf( "Failed to load image!\n" );
		success = false;
	}
	for (int i = 0; i < 10; i++) {
	if( !number[i].loadFromFile( "image/" + std::to_string(i) + ".png" ) )
	{
		printf( "Failed to load image!\n" );
		success = false;
	}
	}

	return success;
}

void close()
{
    bird1Texture.free();
    bird2Texture.free();
    bird3Texture.free();
    background.free();
    land.free();
    uppipe.free();
    lowpipe.free();
    gameOver.free();
    taptap.free();
    for (int i = 0; i < 10; i++) {
        number[i].free();
    }
	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;
	IMG_Quit();
	SDL_Quit();
}

bool checkCollision( SDL_Rect a, SDL_Rect b )
{
    //The sides of the rectangles
    int leftA, leftB;
    int rightA, rightB;
    int topA, topB;
    int bottomA, bottomB;

    //Calculate the sides of rect A
    leftA = a.x;
    rightA = a.x + a.w;
    topA = a.y;
    bottomA = a.y + a.h;

    //Calculate the sides of rect B
    leftB = b.x;
    rightB = b.x + b.w;
    topB = b.y;
    bottomB = b.y + b.h;
    //If any of the sides from A are outside of B
    if( bottomA <= topB )
    {
        return false;
    }

    if( topA >= bottomB )
    {
        return false;
    }

    if( rightA <= leftB )
    {
        return false;
    }

    if( leftA >= rightB )
    {
        return false;
    }

    //If none of the sides from A are outside B
    return true;
}

void Start() {
    if (!start) taptap.render((SCREEN_WIDTH - 240)/2, (SCREEN_WIDTH*4/5 - 208)/2 , 240, 208);
}

void tap(SDL_Event& e) {
    if (e.type == SDL_MOUSEBUTTONDOWN) start = true;
}

int main( int argc, char* args[] )
{
    srand(time(NULL));
	if( !init() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		if( !loadMedia() )
		{
			printf( "Failed to load media!\n" );
		}
		else
		{
			bool quit = false;

			SDL_Event e;

			while( !quit )
			{
				while( SDL_PollEvent( &e ) != 0 )
				{
					if( e.type == SDL_QUIT )
					{
						quit = true;
					}
					if (e.type == SDL_MOUSEBUTTONDOWN) {
                        bird.jump(e);
					}
					tap(e);
				}

				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_RenderClear( gRenderer );
				background.render(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT*4/5);
				bird.render();
                pipe.render();
                moveland();

                if (checkCollision(pipe.cot, bird.chim) || checkCollision(pipe.cot2, bird.chim) || thua ) {
                    gameOver.render( (SCREEN_WIDTH - 500)/2 , (SCREEN_HEIGHT*4/5 - 100)/2 , 500 ,100 );
                    thua = true;
                }

                Start();

				if (checkCollision(pipe.score, bird.chim)) passed = true;


                int k = round(Score/100);
                if (k <= 9) number[k].render((SCREEN_WIDTH-40)/2, SCREEN_HEIGHT/6, 40, 60);
                if (k >= 10) {
                    number[k/10].render((SCREEN_WIDTH - 40)/2 - 22, SCREEN_HEIGHT/6, 40, 60);
                    number[k%10].render((SCREEN_WIDTH - 40)/2 + 22, SCREEN_HEIGHT/6, 40, 60);
                }

				SDL_RenderPresent( gRenderer );
			}
		}
	}

	close();

	return 0;
}




