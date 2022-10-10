from gui_utils import *
import sys
import pygame
import random
from button import Button

pygame.font.init()

screen = pygame.display.set_mode((width, height))
clock = pygame.time.Clock()
pygame.display.set_caption(window_name)
menu_bg = pygame.transform.scale(pygame.image.load(menu_bg_pic_path), (width, height))
game_bg = pygame.transform.scale(pygame.image.load(game_bg_pic_path), (width, height))

selected = [0, 0, 0, 0, 0]
elapsed_time = 0


def reroll_hand():
    global selected, player_dice_buttons, opponent_dice_buttons
    print(selected)
    for i, selected_index in enumerate(selected):
        if selected_index:
            player_dice[i] = random.choice(dice)
    player_dice_buttons, opponent_dice_buttons = init_players_dice_buttons()
    selected = [0, 0, 0, 0, 0]


def main_menu():
    while True:
        screen.blit(menu_bg, (0, 0))

        global elapsed_time
        elapsed_time = 0

        mouse_pos = pygame.mouse.get_pos()

        menu_text = get_font(100).render(main_menu_text, True, color_palette["primary"])
        menu_rect = menu_text.get_rect(center=(width / 2, 100))
        screen.blit(menu_text, menu_rect)

        quick_play_button = Button(
            image=None,
            pos=(width / 2, 350),
            text_input=quick_play_btn_text,
            font=get_font(75),
            base_color=color_palette["secondary"],
            hovering_color=color_palette["primary"]
        )
        blit_rect_behind_button(screen, quick_play_button, 175, color_palette["primary_dark"])
        quit_button = Button(
            image=None,
            pos=(width / 2, 550),
            text_input=quit_btn_text,
            font=get_font(75),
            base_color=color_palette["secondary_dark"],
            hovering_color=color_palette["red"]
        )
        blit_rect_behind_button(screen, quit_button, 175, color_palette["primary_dark"])
        for button in [quick_play_button, quit_button]:
            button.change_color(mouse_pos)
            button.update(screen)

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                sys.exit()
            if event.type == pygame.MOUSEBUTTONDOWN:
                if quick_play_button.check_for_input(mouse_pos):
                    waiting_for_players()
                if quit_button.check_for_input(mouse_pos):
                    pygame.quit()
                    sys.exit()

        clock.tick(60)
        pygame.display.update()


def waiting_for_players():
    while True:
        screen.fill(color_palette["black"])

        mouse_pos = pygame.mouse.get_pos()

        global elapsed_time, waiting_for_players_text
        if elapsed_time < int(pygame.time.get_ticks() / 1000):
            elapsed_time = int(pygame.time.get_ticks() / 1000)
            waiting_for_players_text = change_dots(waiting_for_players_text)
        if elapsed_time >= 10:
            play()

        loading_text = get_font(50).render(waiting_for_players_text, True, color_palette["secondary"])
        loading_rect = loading_text.get_rect(center=(width / 2, height / 2))
        screen.blit(loading_text, loading_rect)

        back = Button(
            image=None,
            pos=(9 * width / 10, 9 * height / 10),
            text_input=back_btn_text,
            font=get_font(25),
            base_color=color_palette["secondary_dark"],
            hovering_color=color_palette["red"]
        )
        blit_rect_behind_button(screen, back, 175, color_palette["primary_dark"])

        back.change_color(mouse_pos)
        back.update(screen)

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                sys.exit()
            if event.type == pygame.MOUSEBUTTONDOWN:
                if back.check_for_input(mouse_pos):
                    main_menu()

        clock.tick(60)
        pygame.display.update()


def play():
    while True:
        screen.blit(game_bg, (0, 0))

        mouse_pos = pygame.mouse.get_pos()

        reroll = Button(
            image=None,
            pos=(9 * width / 10, 8 * height / 10),
            text_input=reroll_btn_text,
            font=get_font(25),
            base_color=color_palette["secondary"],
            hovering_color=color_palette["primary"]
        )
        blit_rect_behind_button(screen, reroll, 175, color_palette["primary_dark"])
        back = Button(
            image=None,
            pos=(9 * width / 10, 9 * height / 10),
            text_input=back_btn_text,
            font=get_font(25),
            base_color=color_palette["secondary_dark"],
            hovering_color=color_palette["red"]
        )
        blit_rect_behind_button(screen, back, 175, color_palette["primary_dark"])

        for button in player_dice_buttons + opponent_dice_buttons + [reroll, back]:
            button.change_color(mouse_pos)
            button.update(screen)

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                sys.exit()
            if event.type == pygame.MOUSEBUTTONDOWN:
                if back.check_for_input(mouse_pos):
                    main_menu()
                if reroll.check_for_input(mouse_pos):
                    reroll_hand()
                for i, button in enumerate(player_dice_buttons):
                    if button.check_for_input(mouse_pos):
                        if button.base_color == color_palette["secondary"]:
                            button.base_color = color_palette["primary"]
                            button.hovering_color = color_palette["red"]
                            selected[i] = 1
                        else:
                            button.base_color = color_palette["secondary"]
                            button.hovering_color = color_palette["green"]
                            selected[i] = 0

        clock.tick(60)
        pygame.display.update()


main_menu()
