from gui.button import Button
import os
os.environ['PYGAME_HIDE_SUPPORT_PROMPT'] = '1'  # Hiding pygame support prompt (must be above import pygame)
import pygame

pygame.font.init()

# GUI UTILITY


def get_font(size, unicode=False):
    if unicode:
        return pygame.font.Font("gui/assets/seguisym.ttf", size)
    else:
        return pygame.font.Font("gui/assets/font.ttf", size)


def init_players_dice_buttons(player_dice, opponent_dice):
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
    offset = 40
    rect = pygame.Surface((button.rect.width + offset, button.rect.height + offset))
    rect.set_alpha(alpha)
    rect.fill(color)
    screen.blit(rect, (button.x_pos - rect.get_width() / 2, button.y_pos - rect.get_height() / 2))


def change_dots(text):
    text = text + "."
    if text[-4] == ".":
        return text[:-4]
    return text


# GUI SETTINGS

width = 1280
height = 720
window_name = "Dice Poker"
menu_bg_pic_path = "gui/assets/menu_background.png"
game_bg_pic_path = "gui/assets/game_background.jpg"
color_palette = {
    "primary": "#d0955a",
    "primary_dark": "#1b0c05",
    "secondary": "#b2aea6",
    "secondary_dark": "#9b9791",
    "green": "#008000",
    "red": "#800000",
    "black": "#000000"
}
login_screen_text1 = "PLEASE ENTER YOUR USERNAME: "
login_screen_text2 = "IP: "
login_screen_text3 = "PORT: "
main_menu_text = "MAIN MENU"
waiting_for_players_text = "WAITING FOR PLAYERS"
create_game_btn_text = "CREATE GAME"
join_game_btn_text = "JOIN GAME"
quit_btn_text = "QUIT"
reroll_btn_text = "REROLL"
back_btn_text = "BACK"
leave_game_btn_text = "LEAVE GAME"
login_btn_text = "LOGIN"
accept_end_of_round_btn_text = "ACCEPT END OF ROUND"

dice = ['', '\u2680', '\u2681', '\u2682', '\u2683', '\u2684', '\u2685']
player_dice_buttons, opponent_dice_buttons = [], []
