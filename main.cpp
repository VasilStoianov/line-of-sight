#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <algorithm>
#define HEIGHT 480
#define WIDTH 640

#define NORTH 0
#define SOUTH 1
#define EAST 2
#define WEST 3

struct Intersect
{
    bool result;
    sf::Vector2f pos;
};

struct sEdge
{
    float sx, sy;
    float ex, ey;
};

struct sCell
{
    int edge_id[4];
    bool edge_exist[4];
    bool exist = false;
};

auto world = new sCell[40 * 40];

void ConvertTileMap(int sx, int sy, int w, int h, float fBlockWidth, int pitch, std::vector<sEdge> &edges);
void CalculateVisibilityPolygon(float ox, float oy, float radius, std::vector<std::tuple<float, float, float>> &visiblePolyongs, std::vector<sEdge> &vecEdges);

sf::Vector2f compareIntersections(const std::vector<sf::Vector2f> &intersections, const sf::Vector2f &mainLineStart);
float cross(sf::Vector2f vec1, sf::Vector2f vec2);
Intersect lineIntersection(sf::Vector2f a, sf::Vector2f b, sf::Vector2f c, sf::Vector2f d);
const int blockWidth = 16;
const int blockHeight = 16;

int main()
{
    // Create a window
    sf::RenderWindow window(sf::VideoMode(40 * 16, 40 * 16, 16), "Ray casting example");
    int numBlocksX = 40;
    int numBlocksY = 40;
    world = new sCell[numBlocksX * numBlocksY];
    std::vector<sEdge> vecEdges;
    std::vector<std::tuple<float, float, float>> visiblePolyongs;

    for (int x = 1; x < (numBlocksX - 1); x++)
    {
        world[1 * numBlocksX + x].exist = true;
        world[(numBlocksX - 2) * numBlocksX + x].exist = true;
    }

    for (int x = 1; x < (numBlocksX - 1); x++)
    {
        world[x * numBlocksX + 1].exist = true;
        world[x * numBlocksX + (numBlocksX - 2)].exist = true;
    }

    while (window.isOpen())
    {
        window.clear();
        sf::Vector2i mousePos(sf::Mouse::getPosition(window));
       
CalculateVisibilityPolygon(mousePos.x, mousePos.y, 1000.0f, visiblePolyongs, vecEdges);
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                window.close();

            if (event.type == sf::Event::MouseButtonPressed)
            {

                if (event.mouseButton.button == sf::Mouse::Right)
                {
                     
                    // mousePos = sf::Mouse::getPosition(window);
                }
            }

            if (event.type == sf::Event::MouseButtonReleased)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    mousePos = sf::Mouse::getPosition(window);
                    int i = ((int)mousePos.y / blockHeight) * numBlocksY + ((int)mousePos.x / (int)blockHeight);
                    world[i].exist = !world[i].exist;
                }
            }
        }
        ConvertTileMap(0, 0, 40, 40, 16.0f, 40, vecEdges);

        for (int x = 0; x < numBlocksX; x++)
        {
            for (int y = 0; y < numBlocksY; y++)
            {
                // Calculate the position of the current block
                if (world[y * numBlocksY + x].exist)
                {
                    sf::RectangleShape rect(sf::Vector2f(blockHeight, blockWidth));
                    rect.setFillColor(sf::Color::Blue);
                    rect.setPosition(sf::Vector2f(x * blockWidth, y * blockWidth));
                    window.draw(rect);
                }

                // Draw or perform operations with the block at position (x, y)
                // Example: sf::RectangleShape block(sf::Vector2f(blockWidth, blockHeight));
                // block.setPosition(x, y);
                // window.draw(block);
            }
        }

        for (auto &e : vecEdges)
        {
            sf::Vertex line[] = {
                sf::Vertex(sf::Vector2f(e.sx, e.sy), sf::Color::White),
                sf::Vertex(sf::Vector2f(e.ex, e.ey), sf::Color::White)};
            window.draw(line, 2, sf::Lines);
        }

        if (visiblePolyongs.size() > 1)
        {

            for (int i = 0; i < visiblePolyongs.size() - 1; i++)
            {
                  sf::VertexArray vertices(sf::TriangleFan, 3);
               

                // resize it to 5 points
                // convex.setPointCount(3);

                // // define the points
                // convex.setPoint(0, sf::Vector2f(mousePos.x, mousePos.y));
                // convex.setPoint(1, sf::Vector2f(std::get<1>(visiblePolyongs[i]), std::get<2>(visiblePolyongs[i])));
                // convex.setPoint(2, sf::Vector2f(std::get<1>(visiblePolyongs[i + 1]), std::get<2>(visiblePolyongs[i + 1])));
                // convex.setFillColor(sf::Color::White);
                // window.draw(convex);

                vertices[0] = sf::Vector2f(mousePos.x, mousePos.y);                                                   // Vertex 1
                vertices[1] = sf::Vector2f(std::get<1>(visiblePolyongs[i]), std::get<2>(visiblePolyongs[i]));         // Vertex 2
                vertices[2] = (sf::Vector2f(std::get<1>(visiblePolyongs[i + 1]), std::get<2>(visiblePolyongs[i + 1]))); // Vertex 3
                for (std::size_t i = 0; i < 3; ++i)
            {
                vertices[i].color = sf::Color::White; // Set the fill color to white
            }
            window.draw(vertices);

                // Create a vertex array to hold the triangle fan
            }
          sf::VertexArray vertices(sf::TriangleFan, 3);
            // define the points
            vertices[0] = sf::Vector2f(mousePos.x, mousePos.y);
            vertices[1] = sf::Vector2f(std::get<1>(visiblePolyongs[visiblePolyongs.size() - 1]),
                                            std::get<2>(visiblePolyongs[visiblePolyongs.size() - 1]));
            vertices[2] = (sf::Vector2f(std::get<1>(visiblePolyongs[0]), std::get<2>(visiblePolyongs[0])));
          
            
            window.draw(vertices);
        }

        window.display();
    }

    return 0;
}

void ConvertTileMap(int sx, int sy, int w, int h, float fBlockWidth, int pitch, std::vector<sEdge> &edges)
{
    // clear map
    edges.clear();
    for (int x = 0; x < w; x++)
        for (int y = 0; y < h; y++)
            for (int j = 0; j < 4; j++)
            {
                world[(y + sy) * pitch + (x + sx)].edge_exist[j] = false;
                world[(y + sy) * pitch + (x + sx)].edge_id[j] = 0;
            }

    for (int x = 1; x < w - 1; x++)
        for (int y = 1; y < h - 1; y++)
        {
            int i = (y + sy) * pitch + (x + sx);     // This
            int n = (y + sy - 1) * pitch + (x + sx); // Northern Neighbour
            int s = (y + sy + 1) * pitch + (x + sx); // Southern Neighbour
            int w = (y + sy) * pitch + (x + sx - 1); // Western Neighbour
            int e = (y + sy) * pitch + (x + sx + 1); // Eastern Neighbour
            if (world[i].exist)
            {
                if (!world[w].exist)
                {
                    if (world[n].edge_exist[WEST])
                    {
                        edges[world[n].edge_id[WEST]].ey += fBlockWidth;
                        world[i].edge_id[WEST] = world[n].edge_id[WEST];
                        world[i].edge_exist[WEST] = true;
                    }
                    else
                    {

                        sEdge edge;
                        edge.sx = (sx + x) * fBlockWidth;
                        edge.sy = (sy + y) * fBlockWidth;
                        edge.ex = edge.sx;
                        edge.ey = edge.sy + fBlockWidth;
                        int edge_id = edges.size();
                        edges.push_back(edge);
                        world[i].edge_id[WEST] = edge_id;
                        world[i].edge_exist[WEST] = true;
                    }
                }

                if (!world[e].exist)
                {
                    // It can either extend it from its northern neighbour if they have
                    // one, or It can start a new one.
                    if (world[n].edge_exist[EAST])
                    {
                        // Northern neighbour has one, so grow it downwards
                        edges[world[n].edge_id[EAST]].ey += fBlockWidth;
                        world[i].edge_id[EAST] = world[n].edge_id[EAST];
                        world[i].edge_exist[EAST] = true;
                    }
                    else
                    {
                        // Northern neighbour does not have one, so create one
                        sEdge edge;
                        edge.sx = (sx + x + 1) * fBlockWidth;
                        edge.sy = (sy + y) * fBlockWidth;
                        edge.ex = edge.sx;
                        edge.ey = edge.sy + fBlockWidth;

                        // Add edge to Polygon Pool
                        int edge_id = edges.size();
                        edges.push_back(edge);

                        // Update tile information with edge information
                        world[i].edge_id[EAST] = edge_id;
                        world[i].edge_exist[EAST] = true;
                    }
                }

                // If this cell doesnt have a northern neignbour, It needs a northern edge
                if (!world[n].exist)
                {
                    // It can either extend it from its western neighbour if they have
                    // one, or It can start a new one.
                    if (world[w].edge_exist[NORTH])
                    {
                        // Western neighbour has one, so grow it eastwards
                        edges[world[w].edge_id[NORTH]].ex += fBlockWidth;
                        world[i].edge_id[NORTH] = world[w].edge_id[NORTH];
                        world[i].edge_exist[NORTH] = true;
                    }
                    else
                    {
                        // Western neighbour does not have one, so create one
                        sEdge edge;
                        edge.sx = (sx + x) * fBlockWidth;
                        edge.sy = (sy + y) * fBlockWidth;
                        edge.ex = edge.sx + fBlockWidth;
                        edge.ey = edge.sy;

                        // Add edge to Polygon Pool
                        int edge_id = edges.size();
                        edges.push_back(edge);

                        // Update tile information with edge information
                        world[i].edge_id[NORTH] = edge_id;
                        world[i].edge_exist[NORTH] = true;
                    }
                }

                // If this cell doesnt have a southern neignbour, It needs a southern edge
                if (!world[s].exist)
                {
                    // It can either extend it from its western neighbour if they have
                    // one, or It can start a new one.
                    if (world[w].edge_exist[SOUTH])
                    {
                        // Western neighbour has one, so grow it eastwards
                        edges[world[w].edge_id[SOUTH]].ex += fBlockWidth;
                        world[i].edge_id[SOUTH] = world[w].edge_id[SOUTH];
                        world[i].edge_exist[SOUTH] = true;
                    }
                    else
                    {
                        // Western neighbour does not have one, so I need to create one
                        sEdge edge;
                        edge.sx = (sx + x) * fBlockWidth;
                        edge.sy = (sy + y + 1) * fBlockWidth;
                        edge.ex = edge.sx + fBlockWidth;
                        edge.ey = edge.sy;

                        // Add edge to Polygon Pool
                        int edge_id = edges.size();
                        edges.push_back(edge);

                        // Update tile information with edge information
                        world[i].edge_id[SOUTH] = edge_id;
                        world[i].edge_exist[SOUTH] = true;
                    }
                }
            }
        }
}

void CalculateVisibilityPolygon(float ox, float oy, float radius, std::vector<std::tuple<float, float, float>> &visiblePolyongs, std::vector<sEdge> &vecEdges)
{
    visiblePolyongs.clear();

    for (auto &e1 : vecEdges)
    {
        for (int i = 0; i < 2; i++)
        {
            float rdx, rdy;
            rdx = (i == 0 ? e1.sx : e1.ex) - ox;
            rdy = (i == 0 ? e1.sy : e1.ey) - ox;

            float base_angle = atan2f(rdy, rdx);
            float ang = 0;
            for (int j = 0; j < 3; j++)
            {
                if (j == 0)
                    ang = base_angle - 0.0001f;
                if (j == 1)
                    ang = base_angle;
                if (j == 2)
                    ang = base_angle + 0.0001f;

                rdx = radius * cosf(ang);
                rdy = radius * sinf(ang);

                float min_t1 = INFINITY;
                float min_px = 0;
                float min_py = 0;
                float min_ang = 0;
                bool valid = false;

                for (auto &e2 : vecEdges)
                {
                    float sdx = e2.ex - e2.sx;
                    float sdy = e2.ey - e2.sy;
                    if (fabs(sdx - rdx) > 0.0f && fabs(sdy - rdy) > 0.0f)
						{
							// t2 is normalised distance from line segment start to line segment end of intersect point
							float t2 = (rdx * (e2.sy - oy) + (rdy * (ox - e2.sx))) / (sdx * rdy - sdy * rdx);
							// t1 is normalised distance from source along ray to ray length of intersect point
							float t1 = (e2.sx + sdx * t2 - ox) / rdx;

							// If intersect point exists along ray, and along line 
							// segment then intersect point is valid
							if (t1 > 0 && t2 >= 0 && t2 <= 1.0f)
							{
								// Check if this intersect point is closest to source. If
								// it is, then store this point and reject others
								if (t1 < min_t1)
								{
									min_t1 = t1;
									min_px = ox + rdx * t1;
									min_py = oy + rdy * t1;
									min_ang = atan2f(min_py - oy, min_px - ox);
									valid = true;
								}
							}
						}
                }
                if (valid)
                {
                    visiblePolyongs.push_back({min_ang, min_px, min_py});
                }
            }
        }
    }
    sort(visiblePolyongs.begin(), visiblePolyongs.end(), [&](const std::tuple<float, float, float> &t1, const std::tuple<float, float, float> &t2)
         { return std::get<0>(t1) < std::get<0>(t2); });
}
