from gui.button import Button
import os
os.environ['PYGAME_HIDE_SUPPORT_PROMPT'] = '1'  # Hiding pygame support prompt (must be above import pygame)
import pygame

# Initialize pygame
pygame.font.init()


# GUI UTILITY


def get_font(size, unicode=False):
    """
    Returns a font object with the given size and unicode support (unicode is used for the dice)
    :param size:
    :param unicode:
    :return:
    """
    if unicode:
        return pygame.font.Font("gui/assets/seguisym.ttf", size)
    else:
        return pygame.font.Font("gui/assets/font.ttf", size)


def init_players_dice_buttons(player_dice, opponent_dice):
    """
    Initializes the dice buttons for the players
    :param player_dice: Player dice
    :param opponent_dice: Opponent dice
    :return: Two lists of dice buttons for the players
    """
    player_dice_buttons_temp, opponent_dice_buttons_temp = [], []

    for i, die in enumerate(player_dice):
        player_dice_buttons_temp.append(Button(
            image=None,
            pos=(width / 5 + i * 150, height - 100),
            text_input=die,
            font=get_font(200, True),
            base_color=color_palette["secondary"],
            hovering_color=color_palette["green"]
        ))

    for i, die in enumerate(opponent_dice):
        opponent_dice_buttons_temp.append(Button(
            image=None,
            pos=(width / 5 + i * 150, 100),
            text_input=die,
            font=get_font(200, True),
            base_color=color_palette["secondary"],
            hovering_color=color_palette["secondary_dark"]
        ))

    return player_dice_buttons_temp, opponent_dice_buttons_temp


def blit_rect_behind_button(screen, button, alpha, color):
    """
    Blits a rectangle behind a button with the given alpha and color
    :param screen: Screen to blit on
    :param button: Button to blit behind
    :param alpha: Alpha channel of the rectangle
    :param color: Color of the rectangle (RGB)
    :return:
    """
    offset = 40
    rect = pygame.Surface((button.rect.width + offset, button.rect.height + offset))
    rect.set_alpha(alpha)
    rect.fill(color)
    screen.blit(rect, (button.x_pos - rect.get_width() / 2, button.y_pos - rect.get_height() / 2))


def change_dots(text):
    """
    Changes the dots at the end of the given text from 0 dots to 3 dots
    Example: "1" -> "1." -> "1.." -> "1..." -> "1"
    :param text: Text to change
    :return: Changed text
    """
    text = text + "."
    if text[-4] == ".":
        return text[:-4]
    return text


# GUI SETTINGS

# Width of the screen
width = 1280
# Height of the screen
height = 720
# Title of the screen
window_name = "Dice Poker"
# Background picture of the main menu screen
menu_bg_pic_path = "gui/assets/menu_background.png"
# Background picture of the game screen
game_bg_pic_path = "gui/assets/game_background.jpg"
# Color palette derived from the background pictures to fit the theme
color_palette = {
    "primary": "#d0955a",
    "primary_dark": "#1b0c05",
    "secondary": "#b2aea6",
    "secondary_dark": "#9b9791",
    "green": "#008000",
    "red": "#800000",
    "black": "#000000"
}
# Main menu text - "MAIN MENU" by default
main_menu_text = "MAIN MENU"
# Waiting for opponent text - "WAITING FOR OPPONENT" by default
waiting_for_players_text = "WAITING FOR OPPONENT"
# Create game button text - "CREATE GAME" by default
create_game_btn_text = "CREATE GAME"
# Join game button text - "JOIN GAME" by default
join_game_btn_text = "JOIN GAME"
# Quit button text - "QUIT" by default
quit_btn_text = "QUIT"
# Reroll button text - "REROLL" by default
reroll_btn_text = "REROLL"
# Back button text - "BACK" by default
back_btn_text = "BACK"
# Leave game button text - "LEAVE GAME" by default
leave_game_btn_text = "LEAVE GAME"
# Accept end of round button text - "ACCEPT" by default
accept_end_of_round_btn_text = "ACCEPT END OF ROUND"

# Dice as unicodes, first string is empty, because there is no die face with 0 value, so 1 - 6 are used
dice = ['', '\u2680', '\u2681', '\u2682', '\u2683', '\u2684', '\u2685']
# Lists of dice buttons for the players
player_dice_buttons, opponent_dice_buttons = [], []
