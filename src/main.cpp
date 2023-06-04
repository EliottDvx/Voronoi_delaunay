#include "application_ui.h"
#include "SDL2_gfxPrimitives.h"
#include <vector>
#include <list>
#include <map>
#include <queue>
#include <algorithm>

#define EPSILON 0.0001f

struct Coords
{
    int x, y;

    bool operator==(const Coords& other)
    {
        return x == other.x and y == other.y;
    }
};

struct Segment
{
    Coords p1, p2;
};

struct Triangle
{
    Coords p1, p2, p3;
    bool complet=false;
};

struct Polygone
{
    std::vector<Coords> points;
    int r = rand() % 255;
    int g = rand() % 255;
    int b = rand() % 255;
};

struct Application
{
    int width, height;
    Coords focus{100, 100};
    enum DrawMode
    {
        SEGMENTS,
        POLYGONES
    };
    DrawMode drawMode = POLYGONES;

    std::vector<Coords> points;
    std::vector<Triangle> triangles;
    std::vector<Segment> voronoi;
    std::vector<Polygone> polygones;
};

bool compareCoords(Coords point1, Coords point2)
{
    if (point1.y == point2.y)
        return point1.x < point2.x;
    return point1.y < point2.y;
}

void drawPoints(SDL_Renderer *renderer, const std::vector<Coords> &points)
{
    for (std::size_t i = 0; i < points.size(); i++)
    {
        filledCircleRGBA(renderer, points[i].x, points[i].y, 3, 240, 240, 23, SDL_ALPHA_OPAQUE);
    }
}

void drawSegments(SDL_Renderer *renderer, const std::vector<Segment> &segments)
{
    for (std::size_t i = 0; i < segments.size(); i++)
    {
        lineRGBA(
            renderer,
            segments[i].p1.x, segments[i].p1.y,
            segments[i].p2.x, segments[i].p2.y,
            240, 240, 20, SDL_ALPHA_OPAQUE);
    }
}

void drawTriangles(SDL_Renderer *renderer, const std::vector<Triangle> &triangles)
{
    for (std::size_t i = 0; i < triangles.size(); i++)
    {
        const Triangle& t = triangles[i];
        trigonRGBA(
            renderer,
            t.p1.x, t.p1.y,
            t.p2.x, t.p2.y,
            t.p3.x, t.p3.y,
            0, 240, 160, SDL_ALPHA_OPAQUE
        );
    }
}

bool comparePoints(const Coords& a, const Coords& b, const Coords& reference)
{
    double angleA = atan2(a.y - reference.y, a.x - reference.x);
    double angleB = atan2(b.y - reference.y, b.x - reference.x);
    
    // Retourne vrai si l'angle de a est plus petit que l'angle de b
    return angleA < angleB;
}

void sortPointsInClockwise(std::vector<Coords>& points)
{
    // Trouver le point avec la plus petite coordonnée y
    Coords reference = points[0];
    for (const Coords& point : points)
    {
        if (point.y < reference.y || (point.y == reference.y && point.x < reference.x))
        {
            reference = point;
        }
    }
    
    // Trier les points en fonction des angles polaires par rapport au point de référence
    sort(points.begin(), points.end(), [&](const Coords& a, const Coords& b){
        return comparePoints(a, b, reference);
    });
}

void drawPolygones(SDL_Renderer *renderer, const std::vector<Polygone> &polygones)
{
    for (std::size_t i = 0; i < polygones.size(); i++)
    {
        const Polygone& p = polygones[i];
        // Trier les points pour qu'ils soient dessinés dans les sens des aiguilles d'une montre
        std::vector<Coords> points = p.points;
        sortPointsInClockwise(points);


        std::vector<Sint16> vx;
        std::vector<Sint16> vy;
        for (std::size_t j = 0; j < points.size(); j++)
        {
            vx.push_back(points[j].x);
            vy.push_back(points[j].y);
        }
        filledPolygonRGBA(renderer, vx.data(), vy.data(), vx.size(), p.r, p.g, p.b, SDL_ALPHA_OPAQUE);
    }
}

void draw(SDL_Renderer *renderer, const Application &app)
{
    /* Remplissez cette fonction pour faire l'affichage du jeu */
    int width, height;
    SDL_GetRendererOutputSize(renderer, &width, &height);

    drawPolygones(renderer, app.polygones);
    drawTriangles(renderer, app.triangles);
    drawPoints(renderer, app.points);
    drawSegments(renderer, app.voronoi);
}

/*
   Détermine si un point se trouve dans un cercle définit par trois points
   Retourne, par les paramètres, le centre et le rayon
*/
bool CircumCircle(
    float pX, float pY,
    float x1, float y1, float x2, float y2, float x3, float y3,
    float *xc, float *yc, float *rsqr
)
{
    float m1, m2, mx1, mx2, my1, my2;
    float dx, dy, drsqr;
    float fabsy1y2 = fabs(y1 - y2);
    float fabsy2y3 = fabs(y2 - y3);

    /* Check for coincident points */
    if (fabsy1y2 < EPSILON && fabsy2y3 < EPSILON)
        return (false);

    if (fabsy1y2 < EPSILON)
    {
        m2 = -(x3 - x2) / (y3 - y2);
        mx2 = (x2 + x3) / 2.0;
        my2 = (y2 + y3) / 2.0;
        *xc = (x2 + x1) / 2.0;
        *yc = m2 * (*xc - mx2) + my2;
    }
    else if (fabsy2y3 < EPSILON)
    {
        m1 = -(x2 - x1) / (y2 - y1);
        mx1 = (x1 + x2) / 2.0;
        my1 = (y1 + y2) / 2.0;
        *xc = (x3 + x2) / 2.0;
        *yc = m1 * (*xc - mx1) + my1;
    }
    else
    {
        m1 = -(x2 - x1) / (y2 - y1);
        m2 = -(x3 - x2) / (y3 - y2);
        mx1 = (x1 + x2) / 2.0;
        mx2 = (x2 + x3) / 2.0;
        my1 = (y1 + y2) / 2.0;
        my2 = (y2 + y3) / 2.0;
        *xc = (m1 * mx1 - m2 * mx2 + my2 - my1) / (m1 - m2);
        if (fabsy1y2 > fabsy2y3)
        {
            *yc = m1 * (*xc - mx1) + my1;
        }
        else
        {
            *yc = m2 * (*xc - mx2) + my2;
        }
    }

    dx = x2 - *xc;
    dy = y2 - *yc;
    *rsqr = dx * dx + dy * dy;

    dx = pX - *xc;
    dy = pY - *yc;
    drsqr = dx * dx + dy * dy;

    return ((drsqr - *rsqr) <= EPSILON ? true : false);
}

void construitVoronoi(Application &app)
{
    // Trier les points selon x
    sort(app.points.begin(), app.points.end(), compareCoords);

    // Vider la liste existante des triangles et des polygones
    app.triangles.clear();

    // Vide la liste des segments de Voronoi
    app.voronoi.clear();

    // Vide la liste des polygones
    app.polygones.clear();

    // Créer un très grand triangle (-1000, -1000), (500, 3000), (1500, -1000)
    Triangle bigT = Triangle({Coords{-1000, -1000}, Coords{500, 3000}, Coords{1500, -1000}});

    // Le rajouter à la liste de triangles déjà créés
    app.triangles.push_back(bigT);

    // Pour chaque point P
    for (std::size_t i = 0; i < app.points.size(); i++){
        Coords P = app.points[i];

        // Créer une liste de segments LS
        std::vector<Segment> LS;

        // Pour chaque triangle T
        for(std::size_t j = 0; j < app.triangles.size(); j++){
            Triangle T = app.triangles[j];

            // Si le cercle circonscrit contient P
            float xc, yc, rsqr;
            if(CircumCircle(P.x, P.y, T.p1.x, T.p1.y, T.p2.x, T.p2.y, T.p3.x, T.p3.y, &xc, &yc, &rsqr)){
                // Récupérer les différents segments de ce triangle dans LS
                LS.push_back(Segment{T.p1, T.p2});
                LS.push_back(Segment{T.p2, T.p3});
                LS.push_back(Segment{T.p3, T.p1});

                // Enlever le triangle T de la liste
                app.triangles.erase(app.triangles.begin() + j);
                j--;
            }
        }

        // Pour chaque segment S de la liste LS
        for(std::size_t j = 0; j < LS.size(); j++){
            Segment S = LS[j];

            // On compare avec les autres segments de la liste
            for(std::size_t k = 0; k < LS.size(); k++){
                if(k == j){
                    continue;
                }

                Segment S2 = LS[k];

                // Si les segments sont identiques
                if(S.p1 == S2.p2 && S.p2 == S2.p1){
                    // On les enlève de la liste
                    LS.erase(LS.begin() + k);
                    LS.erase(LS.begin() + j);
                    j--;
                    break;
                }
            }
        }

        // Pour chaque segment S de la liste LS
        for(std::size_t j = 0; j < LS.size(); j++){
            Segment S = LS[j];

            // On crée un nouveau triangle composé du segment S et du point P
            Triangle T = Triangle({S.p1, S.p2, P});

            app.triangles.push_back(T);
        }
    }

    if(app.drawMode == Application::DrawMode::SEGMENTS){
        // Pour chaque triangle T de la liste
        for(std::size_t i = 0; i < app.triangles.size(); i++){
            Triangle T = app.triangles[i];

            // On regarde quels sont les triangles voisins
            for(std::size_t j = 0; j < app.triangles.size(); j++){
                Triangle T2 = app.triangles[j];

                // Si les triangles sont identiques
                if(T.p1 == T2.p1 && T.p2 == T2.p2 && T.p3 == T2.p3){
                    continue;
                }

                int nbPointsCommuns = 0;
                if(T.p1 == T2.p1 || T.p1 == T2.p2 || T.p1 == T2.p3){
                    nbPointsCommuns++;
                }
                if(T.p2 == T2.p1 || T.p2 == T2.p2 || T.p2 == T2.p3){
                    nbPointsCommuns++;
                }
                if(T.p3 == T2.p1 || T.p3 == T2.p2 || T.p3 == T2.p3){
                    nbPointsCommuns++;
                }

                // Si T1 a deux points en commun avec T2 (et donc un segment en commun)
                if(nbPointsCommuns == 2)
                {
                    // On relie les centres des cercles circonscrits des deux triangles par un segment de Voronoi si le segment n'existe pas déjà
                    float xc1, yc1, rsqr1;
                    float xc2, yc2, rsqr2;
                    CircumCircle(T.p1.x, T.p1.y, T2.p1.x, T2.p1.y, T2.p2.x, T2.p2.y, T2.p3.x, T2.p3.y, &xc1, &yc1, &rsqr1);
                    CircumCircle(T2.p1.x, T2.p1.y, T.p1.x, T.p1.y, T.p2.x, T.p2.y, T.p3.x, T.p3.y, &xc2, &yc2, &rsqr2);

                    Segment S = Segment{Coords{(int)xc1, (int)yc1}, Coords{(int)xc2, (int)yc2}};

                    bool segmentExiste = false;

                    for(std::size_t k = 0; k < app.voronoi.size(); k++){
                        Segment S2 = app.voronoi[k];
                        if(S.p1 == S2.p1 && S.p2 == S2.p2){
                            segmentExiste = true;
                            break;
                        }
                    }
                    if(!segmentExiste){
                        app.voronoi.push_back(S);
                    }
                }
            }
        }
    }
    else if(app.drawMode == Application::DrawMode::POLYGONES){
        // Pour chaque point P de la liste
        for(std::size_t i = 0; i < app.points.size(); i++){
            Coords P = app.points[i];

            // On crée un polygone
            Polygone polygone;

            // On regarde quels sont les triangles qui contiennent P
            for(std::size_t j = 0; j < app.triangles.size(); j++){
                Triangle T = app.triangles[j];

                // Si le triangle contient P
                if(T.p1 == P || T.p2 == P || T.p3 == P){
                    // On insère dans le polygone le centre du cercle circonscrit du triangle
                    float xc, yc, rsqr;
                    CircumCircle(T.p1.x, T.p1.y, T.p1.x, T.p1.y, T.p2.x, T.p2.y, T.p3.x, T.p3.y, &xc, &yc, &rsqr);
                    polygone.points.push_back(Coords{(int)xc, (int)yc});
                }
            }

            // On ajoute le polygone à la liste
            app.polygones.push_back(polygone);
        }
    }
    
}

bool handleEvent(Application &app)
{
    /* Remplissez cette fonction pour gérer les inputs utilisateurs */
    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        if (e.type == SDL_QUIT)
            return false;
        else if (e.type == SDL_WINDOWEVENT_RESIZED)
        {
            app.width = e.window.data1;
            app.height = e.window.data1;
        }
        else if (e.type == SDL_MOUSEWHEEL)
        {
        }
        else if (e.type == SDL_MOUSEBUTTONUP)
        {
            if (e.button.button == SDL_BUTTON_RIGHT)
            {
                app.focus.x = e.button.x;
                app.focus.y = e.button.y;
                app.points.clear();
            }
            else if (e.button.button == SDL_BUTTON_LEFT)
            {
                app.focus.y = 0;
                app.points.push_back(Coords{e.button.x, e.button.y});
                construitVoronoi(app);
            }
        }
        else if (e.type == SDL_KEYDOWN)
        {
            if (e.key.keysym.sym == SDLK_s){
                app.drawMode = Application::DrawMode::SEGMENTS;
                construitVoronoi(app);
            }
            else if (e.key.keysym.sym == SDLK_p){
                app.drawMode = Application::DrawMode::POLYGONES;
                construitVoronoi(app);
            }
        }
    }
    return true;
}

int main(int argc, char **argv)
{
    SDL_Window *gWindow;
    SDL_Renderer *renderer;
    Application app{720, 720, Coords{0, 0}};
    bool is_running = true;

    // Creation de la fenetre
    gWindow = init("Awesome Voronoi", 720, 720);

    if (!gWindow)
    {
        SDL_Log("Failed to initialize!\n");
        exit(1);
    }

    renderer = SDL_CreateRenderer(gWindow, -1, 0); // SDL_RENDERER_PRESENTVSYNC

    /*  GAME LOOP  */
    while (true)
    {
        // INPUTS
        is_running = handleEvent(app);
        if (!is_running)
            break;

        // EFFACAGE FRAME
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // DESSIN
        draw(renderer, app);

        // VALIDATION FRAME
        SDL_RenderPresent(renderer);

        // PAUSE en ms
        SDL_Delay(1000 / 30);
    }

    // Free resources and close SDL
    close(gWindow, renderer);

    return 0;
}
