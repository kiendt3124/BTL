#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <cstdlib>
#include <ctime>
#include <math.h>
#include <string>
#include <SDL_mixer.h>

const double SCREEN_WIDTH = 800;
const double SCREEN_HEIGHT = 800;
int step = 0;
double speedland = 0;
bool checkCollision( SDL_Rect a, SDL_Rect b );
bool thua = false;
bool start = false;
bool tapscreen = false;
bool passed = false;
double Score = 0;
int t1 = 0;
int t3 = 0;
double hlogo = 0;
int press = 0;
bool news = false;
int h = -1, k;
int bestscore = 0;

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;

Mix_Chunk *wing = NULL;
Mix_Chunk *point = NULL;
Mix_Chunk *hit = NULL;
Mix_Chunk *die = NULL;
Mix_Chunk *swoosh = NULL;


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

LTexture birdTexture[3];
LTexture background;
LTexture land;
LTexture uppipe;
LTexture lowpipe;
LTexture gameOver;
LTexture replaybutton[2];
LTexture startbutton[2];
LTexture taptap;
LTexture getReady;
LTexture flappybird;
LTexture board;
LTexture neww;
LTexture number[9];
LTexture medal[4];

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
        double speed = 0.06;
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
            cot.x = 800;
            cot2.x = 800;
            score.x = 800;
        }

        void render() {
                for (int i = 0; i < 3; i++) {
                    if (ls[i][0] + w <= (SCREEN_WIDTH - 55)/4) {
                        cot.y = ls[(i+1)%3][1] - h;
                        cot.x = ls[(i+1)%3][0];
                        cot2.x = ls[(i+1)%3][0];
                        cot2.y = ls[(i+1)%3][1] + b[(i+1)%3];
                    }

                    if (ls[i][0] + w/2 <= (SCREEN_WIDTH - 55)/4) {
                        score.x = ls[(i+1)%3][0];
                        score.y = ls[(i+1)%3][1];
                        score.h = b[(i+1)%3];
                    }

                    if (ls[i][0] <= (SCREEN_WIDTH - 55)/4 && passed) {
                        Score += speed*11/10;
                        passed = false;
                    }

                    if (ls[i][0] <= -100) {
                        ls[i][0] = SCREEN_WIDTH;
                        ls[i][1] = (rand() % 19 + 0) * 25;
                        b[i] = (rand() % 11 + 6) * 25;
                    }
                }

                if (thua) speed = 0;

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
        void reset() {
            for (int i = 0; i<3; i++) {
                 ls[i][0] = (i+1)*d + 300;
                 ls[i][1] = (rand() % 19 + 0) * 25;
                 b[i] = (rand() % 11 + 6) * 25;
            }
            cot.x = 800;
            cot2.x = 800;
            score.x = 800;

            wait = 700;
            speed = 0.06;
        }
};

Pipe pipe;

class Bird
{
    private:
        double w = 55;
        double h = 38.5;
        double x = (SCREEN_WIDTH - w)/4;
        double y = (SCREEN_HEIGHT*4/5 - h)/2 + 50;
        double speed = 0;
        double g = 0.00015;

    public:

        SDL_Rect chim;
        Bird(){
            chim.x = x;
            chim.h = h;
            chim.w = w;
        }

        void update() {
            if (y + h <= SCREEN_HEIGHT*4/5+3 && tapscreen)
            {
                y += speed + 0.5*g;
                chim.y = y;
                speed += g;
            }
            if (checkCollision(pipe.cot2, chim) || checkCollision(pipe.cot, chim)) thua = true;
        }

        void reset() {
            y = (SCREEN_HEIGHT*4/5 - h)/2 + 50;
            speed = 0;
        }


        void jump(SDL_Event& e) {
            if (e.type == SDL_MOUSEBUTTONDOWN && start && !thua) {
                speed = -0.125;
                Mix_PlayChannel( -1, wing, 0 );
            }
        }
        void render() {
            update();
            if (y + h <= SCREEN_HEIGHT*4/5 + 3 && y > 0) {
                birdTexture[step/200].render(x, y, w, h);
                if (!thua) step++;
                if (step >= 599) step = 0;
            }
            else {
                thua = true;
            }
        }
};

Bird bird;


void moveland() {
    if (!thua) speedland -= 0.06;
    land.render(speedland, SCREEN_HEIGHT*4/5, SCREEN_WIDTH*2, SCREEN_HEIGHT/5);
    if (speedland < -800) speedland = 0;
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
		gWindow = SDL_CreateWindow( "FLAPPY BIRD", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
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
				if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
				{
					printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
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

	if( !flappybird.loadFromFile("image/flappybird.png") )
    {
        printf( "Failed to load image!\n" );
        success = false;
    }

    if( !getReady.loadFromFile("image/getReady.png") )
    {
        printf( "Failed to load image!\n" );
        success = false;
    }

    for(int i = 0; i < 2; i++) {
        if( !startbutton[i].loadFromFile( "image/start" + std::to_string(i) + ".png" ) )
        {
            printf( "Failed to load image!\n" );
            success = false;
        }
        if( !replaybutton[i].loadFromFile("image/replay" + std::to_string(i) + ".png" ) )
        {
            printf( "Failed to load image!\n" );
            success = false;
        }
    }

    for(int i = 0; i < 3; i++) {
        if( !birdTexture[i].loadFromFile( "image/bird" + std::to_string(i) + ".png" ) )
        {
            printf( "Failed to load image!\n" );
            success = false;
        }
    }

	if( !background.loadFromFile( "image/background.png" ) )
	{
		printf( "Failed to load image!\n" );
		success = false;
	}

	if( !board.loadFromFile( "image/board.png" ) )
	{
		printf( "Failed to load image!\n" );
		success = false;
	}

	if( !land.loadFromFile( "image/land.png" ) )
	{
		printf( "Failed to load image!\n" );
		success = false;
	}

	if( !neww.loadFromFile( "image/new.png" ) )
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
	for (int i = 0; i < 4; i++) {
        if( !medal[i].loadFromFile( "image/medal" + std::to_string(i) + ".png" ) )
        {
            printf( "Failed to load image!\n" );
            success = false;
        }
	}

	wing = Mix_LoadWAV( "sound/sfx_wing.wav" );

	point = Mix_LoadWAV( "sound/sfx_point.wav" );

	hit = Mix_LoadWAV( "sound/sfx_hit.wav");

	die = Mix_LoadWAV( "sound/sfx_die.wav");

	swoosh = Mix_LoadWAV( "sound/sfx_swooshing.wav");

	return success;
}

void close()
{
    for (int i = 0; i < 3; i++) {
        birdTexture[i].free();
    }
    flappybird.free();
    background.free();
    land.free();
    uppipe.free();
    lowpipe.free();
    gameOver.free();
    taptap.free();
    board.free();
    neww.free();
    for (int i = 0; i < 10; i++) {
        number[i].free();
    }
    for (int i = 0; i < 2; i++) {
        startbutton[i].free();
        replaybutton[i].free();
    }
    for (int i = 0; i < 4; i++) {
        medal[i].free();
    }

    Mix_FreeChunk( wing );
    Mix_FreeChunk( point );
    Mix_FreeChunk( hit );
    Mix_FreeChunk( die );

	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;
	IMG_Quit();
	SDL_Quit();
}

bool checkCollision( SDL_Rect a, SDL_Rect b )
{
    int leftA, leftB;
    int rightA, rightB;
    int topA, topB;
    int bottomA, bottomB;

    leftA = a.x;
    rightA = a.x + a.w;
    topA = a.y;
    bottomA = a.y + a.h;
    leftB = b.x;
    rightB = b.x + b.w;
    topB = b.y;
    bottomB = b.y + b.h;

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
    return true;
}

void GameStart(){
    background.render(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT*4/5);
    moveland();

    hlogo += 0.0005;
    flappybird.render( (SCREEN_WIDTH - 500)/2,(SCREEN_HEIGHT*4/5 - 100)/3 + 25*cos(hlogo), 500, 100);
    step++;
    birdTexture[step/300].render((SCREEN_WIDTH - 500)/2 + 500, (SCREEN_HEIGHT*4/5 - 38.5)/3 + 25*cos(hlogo), 55, 38.5);
    if (step >= 899) step = 0;
    startbutton[press].render( (SCREEN_WIDTH - 150)/2 , (SCREEN_HEIGHT - 75)/2 + 100 , 150, 75);

}

void Button(SDL_Event e) {
    int x, y;
    SDL_GetMouseState(&x, &y);
    if ( x < (SCREEN_WIDTH - 150)/2 || x > (SCREEN_WIDTH - 150)/2 + 150 || y < (SCREEN_HEIGHT - 75)/2 + 100 || y > (SCREEN_HEIGHT - 75)/2 + 175 ) press = 0;
    else {
        press = 1;
        if (e.type == SDL_MOUSEBUTTONUP && !start && t1 > 2000) {
            Mix_PlayChannel( -1, swoosh, 0 );
            start = true;
            bird.reset();
            t1 = 0;
        }
        if (e.type == SDL_MOUSEBUTTONUP && thua && t3 > 2000) {
            Mix_PlayChannel( -1, swoosh, 0 );
            pipe.reset();
            thua = false;
            start = false;
            Score = 0;
            tapscreen = false;
            t3 = 0;
            news = false;
        }
    }
}

void GamePlay() {
    background.render(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT*4/5);
    bird.render();
    if (!tapscreen) {
        getReady.render((SCREEN_HEIGHT - 400)/2, (SCREEN_HEIGHT - 100)/2 - 200, 400, 100);
        taptap.render((SCREEN_WIDTH - 200)/2, (SCREEN_WIDTH*4/5 - 140)/2 + 50 , 200, 140);
    }

    if (tapscreen) pipe.render();

    moveland();

    if (checkCollision(pipe.score, bird.chim)) passed = true;

    k = round(Score/100);
    if (k == h) Mix_PlayChannel( -1, point, 0 );
    h = k+1;
    if (k > bestscore) {
        bestscore = k;
        news = true;
    }
    if (tapscreen && !thua) {
        if (k <= 9) number[k].render((SCREEN_WIDTH-40)/2, SCREEN_HEIGHT/6, 40, 60);
        if (k >= 10) {
            number[k/10].render((SCREEN_WIDTH - 40)/2 - 22, SCREEN_HEIGHT/6, 40, 60);
            number[k%10].render((SCREEN_WIDTH - 40)/2 + 22, SCREEN_HEIGHT/6, 40, 60);
        }
    }
}

void tap(SDL_Event& e) {
    if (e.type == SDL_MOUSEBUTTONDOWN && start) tapscreen = true;
}

void GameOver() {
    if (thua) t3++;
    if (t3 == 1) Mix_PlayChannel( -1, hit, 0 );
    if (t3>1 && t3<500) SDL_RenderClear( gRenderer );
    if (t3 == 1000) Mix_PlayChannel( -1, die, 0 );
    if (t3 > 2000){
            gameOver.render( (SCREEN_WIDTH - 500)/2, (SCREEN_HEIGHT*4/5 - 100)/2 - 200, 500, 100 );
            replaybutton[press].render( (SCREEN_WIDTH - 150)/2 , (SCREEN_HEIGHT - 75)/2 + 100 , 150, 75);
            board.render((SCREEN_WIDTH - 400)/2, (SCREEN_HEIGHT - 205)/2 - 75, 400, 205);

            if (k <= 9) number[k].render(535, 290, 20, 30);
            if (k >= 10) {
                number[k/10].render(510, 290, 20, 30);
                number[k%10].render(535, 290, 20, 30);
            }
            if (bestscore <= 9) number[bestscore].render(535, 360, 20, 30);
            if (bestscore >= 10) {
                number[bestscore/10].render(510, 360, 20, 30);
                number[bestscore%10].render(535, 360, 20, 30);
            }

            if (news) neww.render(440, 325, 58, 25);

            if (k >= 10 && k < 20) medal[0].render(246, 295, 77, 77);
            else if (k >= 20 && k < 30) medal[1].render(246, 295, 77, 77);
            else if (k >= 30 && k < 40) medal[2].render(246, 295, 77, 77);
            else if (k >= 40) medal[3].render(246, 295, 77, 77);
    }
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

					Button(e);

                    bird.jump(e);

					tap(e);
				}

				SDL_RenderClear( gRenderer );

                if (!start) {
                    t1++;
                    if (t1 > 1000) GameStart();
                }

				if (start)
                {
                    GamePlay();
                }

                if (thua) GameOver();

				SDL_RenderPresent( gRenderer );
			}
		}
	}

	close();

	return 0;
}




