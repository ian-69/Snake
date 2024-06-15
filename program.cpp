#include "splashkit.h"

// Define constants for various element sizes
#define SNAKE_SIZE 30
#define APPLE_SIZE 20
#define OBSTACLE_SIZE 40

// Define a structure to represent a point with x and y coordinates
struct point
{
    double x, y;
};

// Define a structure to represent the snake
struct snake
{
    point body[SNAKE_SIZE]; // Array of points representing the snake's body segments
    int length;             // Current length of the snake
    point direction;        // Current direction of movement
    double speed;           // Movement speed of the snake
};

// Define a structure to represent an apple
struct apple
{
    point position; // Position of the apple
};

// Define a structure to represent an obstacle
struct obstacle
{
    point position; // Position of the obstacle
};

// Define game states
enum game_state
{
    MENU,
    PLAYING,
    GAME_OVER
};

// Initialize the current game state to MENU
game_state current_state = MENU;

// Function to update the snake's position and movement
void update_snake(snake &s)
{
    // Update the position of the snake's head based on direction and speed
    point new_head;
    new_head.x = s.body[0].x + s.direction.x * s.speed;
    new_head.y = s.body[0].y + s.direction.y * s.speed;

    // Shift the positions of body segments to create movement effect
    for (int i = s.length - 1; i > 0; i--)
    {
        s.body[i] = s.body[i - 1];
    }


    // Wrap around from screen edges (left to right, top to bottom)
    // to create the illusion of a continuous play area
    // (snakes appear on the opposite side when crossing edges)
    // This code ensures the snake doesn't disappear off-screen.
    if (new_head.x < 0)
    {
        new_head.x = screen_width() - SNAKE_SIZE;
    }
    else if (new_head.x >= screen_width())
    {
        new_head.x = 0;
    }

    // Adjust y-coordinate in a similar manner to wrap vertically
    if (new_head.y < 0)
    {
        new_head.y = screen_height() - SNAKE_SIZE;
    }
    else if (new_head.y > screen_height() - SNAKE_SIZE)
    {
        new_head.y = 0;
    }

    // Check for collision with snake's body
    for (int i = 1; i < s.length; i++)
    {
        if (new_head.x == s.body[i].x && new_head.y == s.body[i].y)
        {
            // Snake head collided with its body
            s.length = i; // Cut off the tail from the collision point
            break;
        }
    }

    // Update the snake's head position
    s.body[0] = new_head;
}

// Function to check collision between the snake and an apple
bool has_collided_with_apple(const snake &s, const apple &a)
{
    // Check if the distance between snake's head and apple's position is within a certain range
    return abs(s.body[0].x - a.position.x) < SNAKE_SIZE && abs(s.body[0].y - a.position.y) < SNAKE_SIZE;
}

// Function to check collision between the snake and an obstacle
bool has_collided_with_obstacle(const snake &s, const obstacle &o)
{
    // Check if the distance between snake's head and obstacle's position is within a certain range
    return abs(s.body[0].x - o.position.x) < SNAKE_SIZE && abs(s.body[0].y - o.position.y) < SNAKE_SIZE;
}

// Function to generate a new position for an item (apple or obstacle)
void generate_new_item(point &item)
{
    // Generate random coordinates within the screen bounds
    item.x = rnd(screen_width() - SNAKE_SIZE);
    item.y = rnd(screen_height() - SNAKE_SIZE);
}

// Function to draw the main menu
void draw_menu()
{
    // Draw the menu background, title, and instructions
    draw_bitmap("images/menu.png", 0, 0);
    draw_text("Snake Game", COLOR_BLACK, "father", 105, 245, 135);
    draw_text("Snake Game", COLOR_DARK_GREEN, "father", 100, 250, 140);
    draw_text("Press ENTER to Play", COLOR_BLACK, "father", 40, 280, 250);
    draw_text("Press Q to Quit", COLOR_BLACK, "father", 40, 290, 300);
    play_sound_effect("game_opening");
}

// Function to draw the game over screen
void draw_game_over(int score)
{
    // Draw the game over screen with score and instructions
    draw_bitmap("images/menu.png", 0, 0);
    draw_text("Game Over", COLOR_BLACK, "father", 105, 275, 135);
    draw_text("Game Over", COLOR_RED, "father", 100, 280, 140);
    draw_text("Score: " + std::to_string(score), COLOR_GOLD, "father", 40, 290, 260);
    draw_text("Press ENTER to Play Again", COLOR_BLACK, "father", 40, 280, 320);
    draw_text("Press Q to Quit", COLOR_WHITE, "father", 40, 290, 380);
    play_sound_effect("game_over");
}

int main()
{
    // Initialize the game window and load resources
    open_window("Exciting Snake Game", 800, 600);
    load_sound_effect("game_over", "sound_effects/game_over.mp3");
    load_sound_effect("obstacle_hit", "sound_effects/obstacle_hit.mp3");
    load_sound_effect("coin_collect", "sound_effects/coin_collect.mp3");
    load_sound_effect("game_opening", "sound_effects/game_opening.mp3");
    load_font("father", "TheGodfather-rgRK.ttf");

    // Initialize the snake's properties
    snake player;
    player.length = 4;
    player.body[0].x = 400;
    player.body[0].y = 300;
    player.direction.x = 0;
    player.direction.y = -1;
    player.speed = 2;

    // Initialize the apple's position
    apple my_apple;
    generate_new_item(my_apple.position);

    // Initialize the obstacle's position
    obstacle my_obstacle;
    generate_new_item(my_obstacle.position);

    int score = 0; // Initialize the score

    // Main game loop
    while (not quit_requested())
    {
        process_events();

        if (current_state == MENU)
        {
            // Check for user input in the menu state
            if (key_typed(RETURN_KEY))
            {
                score = 0;
                current_state = PLAYING;
            }

            if (key_typed(Q_KEY))
            {
                close_window("Exciting Snake Game");
                return 0;
            }

            draw_menu(); // Draw the menu screen
        }
        else if (current_state == PLAYING)
        {
            // Update snake's direction based on user input
            // Note: Only one direction change is allowed per frame to prevent reverse movement
            if (key_down(UP_KEY) && player.direction.y != 1)
            {
                player.direction.x = 0;
                player.direction.y = -1;
            }
            if (key_down(DOWN_KEY) && player.direction.y != -1)
            {
                player.direction.x = 0;
                player.direction.y = 1;
            }
            if (key_down(LEFT_KEY) && player.direction.x != 1)
            {
                player.direction.x = -1;
                player.direction.y = 0;
            }
            if (key_down(RIGHT_KEY) && player.direction.x != -1)
            {
                player.direction.x = 1;
                player.direction.y = 0;
            }

            update_snake(player); // Update the snake's position

            // Check for collisions with apple and obstacle
            if (has_collided_with_apple(player, my_apple))
            {
                player.length += 2; // Increase the snake's length
                player.speed += 0.7; // Increase the snake's speed
                generate_new_item(my_apple.position); // Generate a new apple
                generate_new_item(my_obstacle.position); // Generate a new obstacle
                play_sound_effect("coin_collect"); // Play sound effect
                score++; // Increment the score
            }

            if (has_collided_with_obstacle(player, my_obstacle))
            {
                play_sound_effect("obstacle_hit"); // Play sound effect
                current_state = GAME_OVER; // Change game state to GAME_OVER
            }

            clear_screen(COLOR_BLACK); // Clear the screen
            draw_bitmap("images/background.png", 0, 0); // Draw the background image

            // Draw the snake's body segments
            for (int i = 0; i < player.length; i++)
            {
                fill_ellipse(COLOR_GREEN, player.body[i].x, player.body[i].y, SNAKE_SIZE, SNAKE_SIZE);
            }

            // Draw the apple and obstacle
            draw_bitmap("images/apple.png", my_apple.position.x, my_apple.position.y);
            draw_bitmap("images/obstacle.png", my_obstacle.position.x, my_obstacle.position.y);
        }
        else if (current_state == GAME_OVER)
        {
            draw_game_over(score); // Draw the game over screen

            // Check for user input in the game over state
            if (key_typed(RETURN_KEY))
            {
                // Reset game variables and return to the PLAYING state
                player.length = 4;
                player.speed = 2;
                player.body[0].x = 400;
                player.body[0].y = 300;
                score = 0;
                current_state = PLAYING;
            }

            if (key_typed(Q_KEY))
            {
                close_window("Exciting Snake Game");
                return 0;
            }
        }

        refresh_screen(60); // Refresh the screen at 60 frames per second
    }

    close_audio(); // Close audio resources
    return 0;
}
