#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game.h"
#include "utils.h"

// Map display functions
static void displayMap(GameState* g) {
    if (!g->rooms) return;
    
    // Find bounds
    int minX = 0, maxX = 0, minY = 0, maxY = 0;
    for (Room* r = g->rooms; r; r = r->next) {
        if (r->x < minX) minX = r->x;
        if (r->x > maxX) maxX = r->x;
        if (r->y < minY) minY = r->y;
        if (r->y > maxY) maxY = r->y;
    }
    
    int width = maxX - minX + 1;
    int height = maxY - minY + 1;
    
    // Create grid
    int** grid = malloc(height * sizeof(int*));
    for (int i = 0; i < height; i++) {
        grid[i] = malloc(width * sizeof(int));
        for (int j = 0; j < width; j++) grid[i][j] = -1;
    }
    
    for (Room* r = g->rooms; r; r = r->next)
        grid[r->y - minY][r->x - minX] = r->id;
    
    printf("=== SPATIAL MAP ===\n");
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (grid[i][j] != -1) printf("[%2d]", grid[i][j]);
            else printf("    ");
        }
        printf("\n");
    }
    
    for (int i = 0; i < height; i++) free(grid[i]);
    free(grid);
}
//comp the items according to the order given to us
int compareItems(void* a, void* b) {
    Item* i1 = (Item*)a;
    Item* i2 = (Item*)b;

    int nameCmp = strcmp(i1->name, i2->name);
    if (nameCmp != 0)
        return nameCmp;

    if (i1->value != i2->value)
        return i1->value - i2->value;

    if (i1->type != i2->type)
        return i1->type - i2->type;

    return 0;  //identical items
}
//just printing the data according to the formart
void printItem(void* data) {
    Item* i = (Item*)data;

    char* type;
    switch (i->type) {
    case ARMOR: type = "ARMOR"; break;
    case SWORD: type = "SWORD"; break;
    default:    type = "UNKNOWN"; break;
    }

    printf("[%s] %s - Value: %d\n",
           type, i->name, i->value);
}
//free the item
void freeItem(void* data) {
    if (!data) return;
    Item* i = (Item*)data;
    free(i->name);
    free(i);
}

//exacly like the compareItems func but we added another case (hp)
int compareMonsters(void* a, void* b) {
    Monster* m1 = (Monster*)a;
    Monster* m2 = (Monster*)b;

    int nameCmp = strcmp(m1->name, m2->name);
    if (nameCmp != 0)
        return nameCmp;

    if (m1->attack != m2->attack)
        return m1->attack - m2->attack;

    if (m1->hp != m2->hp)
        return m1->hp - m2->hp;


    if (m1->type != m2->type)
        return m1->type - m2->type;

    return 0;
}
//exactaly like printItem but we added some data
void printMonster(void* data) {
    Monster* m = (Monster*)data;

    char* type;
    switch (m->type) {
    case PHANTOM: type = "Phantom"; break;
    case SPIDER:  type = "Spider";  break;
    case DEMON:   type = "Demon";   break;
    case GOLEM:   type = "Golem";   break;
    case COBRA:   type = "Cobra";   break;
    default:      type = "Unknown"; break;
    }

    printf("[%s] Type: %s, Attack: %d, HP: %d\n",
           m->name, type, m->attack, m->hp);
}
//exactaly like freeing an item
void freeMonster(void* data) {
    if (!data) return;
    Monster* m = (Monster*)data;
    free(m->name);
    free(m);
}


void initPlayer(GameState* g) {
    if (!g->rooms) {
        printf("Create rooms first\n");
        return;
    }
    if (g->player) return;

    Player* p = malloc(sizeof(Player));
    if (!p) exit(1);

    p->maxHp = g->configMaxHp;
    p->hp = g->configMaxHp;
    p->baseAttack = g->configBaseAttack;

    p->bag = createBST(compareItems, printItem, freeItem);
    p->defeatedMonsters = createBST(compareMonsters, printMonster, freeMonster);

    // player must start at room ID 0
    p->currentRoom = findRoomById(g, 0);
    p->currentRoom->visited = 1;

    g->player = p;
}
//finding a room by an id
Room* findRoomById(GameState* g, int id) {
    for (Room* r = g->rooms; r; r = r->next) {
        if (r->id == id)
            return r;
    }
    return NULL;
}
//same logic as findRoomById, but we search by coords
Room* findRoomByCoords(GameState* g, int x, int y) {
    for (Room* r = g->rooms; r; r = r->next) {
        if (r->x == x && r->y == y)
            return r;
    }
    return NULL;
}
//printing the room according to the format
void printRoomLegend(GameState* g) {
    printf("=== ROOM LEGEND ===\n");
    for (int id = g->roomCount - 1; id >= 0; --id) {
        Room* r = findRoomById(g, id);
        char m = (r && r->monster) ? 'V' : 'X';
        char i = (r && r->item) ? 'V' : 'X';
        printf("ID %d: [M:%c] [I:%c]\n", id, m, i);
    }
    printf("===================\n");
}
void addRoom(GameState* g) {
    int x = 0, y = 0;
    int id = g->roomCount;

    // fist case - first room
    if (!g->rooms) {
        x = 0;
        y = 0;
    } else {
        displayMap(g);
        printRoomLegend(g);

        int attachId = getInt("Attach to room ID:\n");
        Room* base = findRoomById(g, attachId);
        if (!base) return; // we can assume that but just in case

        int dir = getInt("Direction (0=Up,1=Down,2=Left,3=Right): ");

        x = base->x;
        y = base->y;
        if (dir == 0) y -= 1;      // Up
        else if (dir == 1) y += 1; // Down
        else if (dir == 2) x -= 1; // Left
        else if (dir == 3) x += 1; // Right

        if (findRoomByCoords(g, x, y)) {
            printf("Room exists there\n");
            return;
        }
    }

    // create a new room
    Room* r = malloc(sizeof(Room));
    r->id = id;
    r->x = x;
    r->y = y;
    r->visited = 0;
    r->monster = NULL;
    r->item = NULL;

    // adding the room to the linked list
    r->next = g->rooms;
    g->rooms = r;
    g->roomCount++;

    // adding a monster
    int addM = getInt("Add monster? (1=Yes, 0=No):\n");
    if (addM == 1) {
        Monster* m = malloc(sizeof(Monster));
        m->name = getString("Monster name:\n");
        m->type = getInt("Type (0-4):\n");
        m->hp = getInt("HP:\n");
        m->maxHp = m->hp;
        m->attack = getInt("Attack:\n");
        r->monster = m;
    }

    // adding an item
    int addI = getInt("Add item? (1=Yes, 0=No):\n");
    if (addI == 1) {
        Item* i = malloc(sizeof(Item));
        i->name = getString("Item name:\n");
        i->type = getInt("Type (0=Armor, 1=Sword):\n");
        i->value = getInt("Value:\n");
        r->item = i;
    }

    printf("Created room %d at (%d,%d)\n", r->id, r->x, r->y);
}
//preview of the game at the moment according to the format
void gamePreview(GameState* g) {
    Player* p = g->player;
    Room* r = p->currentRoom;

    displayMap(g);
    printRoomLegend(g);

    printf("--- Room %d ---\n", r->id);

    if (r->monster) printf("Monster: %s (HP:%d)\n", r->monster->name, r->monster->hp);
    if (r->item) printf("Item: %s\n", r->item->name);

    printf("HP: %d/%d\n", p->hp, p->maxHp);
}

//checking if all the monsters are killed and if the game is over
int checkVictory(GameState* g) {
    for (Room* r = g->rooms; r; r = r->next) {
        if (r->visited == 0) return 0;
        if (r->monster != NULL) return 0;
    }
    return 1;
}
//the main func that operates the game itself
void playGame(GameState* g)
{
    if (!g->player) {
        printf("Init player first\n");
        return;
    }

    while (1) {
        // checking for victory every loop
        if (checkVictory(g)) {
            printf("***************************************\n");
            printf("             VICTORY!                  \n");
            printf(" All rooms explored. All monsters defeated. \n");
            printf("***************************************\n");
            freeGame(g);
            exit(0);
        }

        gamePreview(g);
        printf("1.Move 2.Fight 3.Pickup 4.Bag 5.Defeated 6.Quit\n");

        int choice = getInt("");

        Player* p = g->player;
        Room* cur = p->currentRoom;

        if (choice == 6) {
            return; // Quit
        }

        // MOVE
        if (choice == 1) {
            if (cur->monster) {
                printf("Kill monster first\n");
                continue;
            }

            int dir = getInt("Direction (0=Up,1=Down,2=Left,3=Right): ");

            int nx = cur->x, ny = cur->y;
            if (dir == 0) ny -= 1;       // Up: y-1
            else if (dir == 1) ny += 1;  // Down: y+1
            else if (dir == 2) nx -= 1;  // Left
            else if (dir == 3) nx += 1;  // Right

            Room* next = findRoomByCoords(g, nx, ny);
            if (!next) {
                printf("No room there\n");
                continue;
            }

            p->currentRoom = next;
            next->visited = 1;
            continue;
        }

        // FIGHT
        if (choice == 2) {
            if (!cur->monster) {
                printf("No monster\n");
                continue;
            }

            Monster* m = cur->monster;

            while (1) {
                // Player attacks
                m->hp -= p->baseAttack;
                if (m->hp < 0) m->hp = 0;
                printf("You deal %d damage. Monster HP: %d\n", p->baseAttack, m->hp);

                if (m->hp == 0) {
                    printf("Monster defeated!\n");

                    // restore original HP for the defeated tree
                    m->hp = m->maxHp;

                    p->defeatedMonsters->root = bstInsert(p->defeatedMonsters->root, m, p->defeatedMonsters->compare);

                    cur->monster = NULL;
                    break;
                }

                // Monster attacks
                p->hp -= m->attack;
                if (p->hp < 0) p->hp = 0;
                printf("Monster deals %d damage. Your HP: %d\n", m->attack, p->hp);

                if (p->hp == 0) {
                    freeGame(g);
                    printf("--- YOU DIED ---\n");
                    exit(0);
                }
            }

            continue;
        }

        // PICKUP
        if (choice == 3) {
            if (cur->monster) {
                printf("Kill monster first\n");
                continue;
            }

            if (!cur->item) {
                printf("No item here\n");
                continue;
            }

            // checking for duplicates
            if (bstFind(p->bag->root, cur->item, p->bag->compare) != NULL) {
                printf("Duplicate item.\n");
                continue; // the item stays in the room
            }

            printf("Picked up %s\n", cur->item->name);

            // the tree will point at the item from now on
            p->bag->root = bstInsert(p->bag->root, cur->item, p->bag->compare);
            cur->item = NULL;

            continue;
        }

        // BAG
        if (choice == 4) {
            printf("=== INVENTORY ===\n");
            printf("1.Preorder 2.Inorder 3.Postorder\n");

            int t = getInt("");
            if (t == 1) bstPreorder(p->bag->root, p->bag->print);
            else if (t == 2) bstInorder(p->bag->root, p->bag->print);
            else if (t == 3) bstPostorder(p->bag->root, p->bag->print);

            continue;
        }

        // DEFEATED
        if (choice == 5) {
            printf("=== DEFEATED MONSTERS ===\n");
            printf("1.Preorder 2.Inorder 3.Postorder\n");

            int t = getInt("");
            if (t == 1) bstPreorder(p->defeatedMonsters->root, p->defeatedMonsters->print);
            else if (t == 2) bstInorder(p->defeatedMonsters->root, p->defeatedMonsters->print);
            else if (t == 3) bstPostorder(p->defeatedMonsters->root, p->defeatedMonsters->print);

        }
    }
}

void freeGame(GameState* g) {
    if (!g) return;

    // free player and the BTS's
    if (g->player) {
        Player* p = g->player;

        if (p->bag) {
            bstFree(p->bag->root, p->bag->freeData);   // frees nodes and each item
            free(p->bag);
            p->bag = NULL;
        }

        if (p->defeatedMonsters) {
            bstFree(p->defeatedMonsters->root, p->defeatedMonsters->freeData); // frees nodes and each monster
            free(p->defeatedMonsters);
            p->defeatedMonsters = NULL;
        }

        free(p);
        g->player = NULL;
    }

    // free all rooms and what they contain
    Room* r = g->rooms;
    while (r) {
        Room* next = r->next;

        if (r->item) {
            freeItem(r->item);
            r->item = NULL;
        }
        if (r->monster) {
            freeMonster(r->monster);
            r->monster = NULL;
        }

        free(r);
        r = next;
    }

    g->rooms = NULL;
    g->roomCount = 0;
}