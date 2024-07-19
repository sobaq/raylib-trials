/// Like little networks sharing their packets.
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <raylib.h>
#include <raymath.h>

#define NETWORKS 10
#define NETWORK_RECT_SIZE 50

#define MAX_PACKETS 50
#define PACKET_RECT_SIZE 10
#define PACKET_SPEED 200

struct network {
    Rectangle rect;
    int pkts_recv;    // # of packets received
    short chattiness;
    Color colour;
};

struct packet {
    int dest;
    Rectangle rect;
    bool recvd;
};

Color random_colour(void)
{
    static int colour = -1;
    colour = (colour + 1) % 18;
    switch (colour) {
        case 0:  return YELLOW;
        case 1:  return GOLD;
        case 2:  return ORANGE;
        case 3:  return PINK;
        case 4:  return RED;
        case 5:  return MAROON;
        case 6:  return GREEN;
        case 7:  return LIME;
        case 8:  return DARKGREEN;
        case 9:  return SKYBLUE;
        case 10: return BLUE;
        case 11: return PURPLE;
        case 12: return VIOLET;
        case 13: return DARKPURPLE;
        case 14: return BEIGE;
        case 15: return BROWN;
        case 16: return DARKBROWN;
        default: return MAGENTA;
    }
}

int main(void)
{
    // Raylib does this itself, but in InitWindow, which I'd like to call after
    // this setup.
    SetRandomSeed(time(NULL));

    int wind_width = 900, wind_height = 600;

    struct network *networks = malloc(sizeof (struct network) * NETWORKS);

    int pkt_counter = 0;
    struct packet *pkts = malloc(sizeof (struct packet) * MAX_PACKETS);

    for (int i = 0; i < NETWORKS; i++)
    {
        Rectangle rect;
        do {
            rect = (Rectangle){
                .x = GetRandomValue(10, wind_width - NETWORK_RECT_SIZE),
                .y = GetRandomValue(10, wind_height - NETWORK_RECT_SIZE),
                .width = NETWORK_RECT_SIZE,
                .height = NETWORK_RECT_SIZE,
            };

            for (int j = 0; j < i; j++)
                if (CheckCollisionRecs(rect, networks[j].rect))
                    goto collides;
            break;

            collides:
        } while (true);

        networks[i] = (struct network){
            .rect = rect,
            .pkts_recv = 0,
            .chattiness = GetRandomValue(25, 75),
            .colour = random_colour(),
        };
    }

    double pkt_clock = 0;
    InitWindow(wind_width, wind_height, "sharing");
    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
    BeginDrawing();
    ClearBackground(RAYWHITE);

        // Draw networks
        for (int i = 0; i < NETWORKS; i++)
        {
            char pkt_buf[32];
            sprintf(pkt_buf, "%d", networks[i].pkts_recv);

            DrawRectangleRec(networks[i].rect, networks[i].colour);
            DrawText(
                pkt_buf,
                networks[i].rect.x + (networks[i].rect.width / 2) - (MeasureText(pkt_buf, 40) / 2),
                networks[i].rect.y - GetFontDefault().baseSize * 4,
                40,
                BLACK
            );
        }

        // Draw and move packets
        for (int i = 0; i < pkt_counter; i++)
        {
            DrawRectangleRec(pkts[i].rect, BLACK);

            struct network *dest = &networks[pkts[i].dest];
            if (CheckCollisionRecs(pkts[i].rect, dest->rect)) {
                dest->pkts_recv += 1;
                pkts[i].recvd = true;
            } else {
                // TODO: Don't really like this
                Vector2 pktvec = (Vector2){ .x = pkts[i].rect.x, .y = pkts[i].rect.y, };
                Vector2 ntwvec = (Vector2){
                    .x = dest->rect.x + (dest->rect.width / 2),
                    .y = dest->rect.y + (dest->rect.height / 2),
                };
                Vector2 res = Vector2MoveTowards(pktvec, ntwvec, PACKET_SPEED * GetFrameTime());
                pkts[i].rect.x = res.x, pkts[i].rect.y = res.y;
            }
        }

        // Potentially spawn packets
        if (pkt_clock > 2.)
        {
            pkt_clock = 0;
            for (int i = 0; i < NETWORKS; i++)
            {
                bool send_pkt = pkt_counter < MAX_PACKETS
                    && GetRandomValue(0, 100) > networks[i].chattiness;
                if (send_pkt)
                {
                    int target_idx;
                    while ((target_idx = GetRandomValue(0, NETWORKS - 1)) == i);

                    pkts[pkt_counter++] = (struct packet){
                        .dest = target_idx,
                        .rect = (Rectangle){
                            .x = networks[i].rect.x + (networks[i].rect.width / 2),
                            .y = networks[i].rect.y + (networks[i].rect.height / 2),
                            .width = PACKET_RECT_SIZE,
                            .height = PACKET_RECT_SIZE,
                        },
                        .recvd = false,
                    };
                }
            }
        }

        // Remove received packets
        int j = 0;
        for (int i = 0; i < pkt_counter; i++)
            if (!pkts[i].recvd)
                pkts[j++] = pkts[i];
        pkt_counter = j;

    pkt_clock += GetFrameTime();
    EndDrawing();
    }
    
    CloseWindow();
    free(networks);
    free(pkts);
    
    return 0;
}
