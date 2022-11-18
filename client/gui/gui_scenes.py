import sys
from gui.gui_utils import *
from gui.button import Button

pygame.font.init()

screen = pygame.display.set_mode((width, height))
clock = pygame.time.Clock()
pygame.display.set_caption(window_name)
menu_bg = pygame.transform.scale(pygame.image.load(menu_bg_pic_path), (width, height))
game_bg = pygame.transform.scale(pygame.image.load(game_bg_pic_path), (width, height))

selected = [0, 0, 0, 0, 0]
elapsed_time = 0

gui_input = ""


def main_menu(args):
    screen.blit(menu_bg, (0, 0))

    global elapsed_time, gui_input
    elapsed_time = 0
    gui_input = ""

    mouse_pos = pygame.mouse.get_pos()

    menu_text = get_font(100).render(main_menu_text, True, color_palette["primary"])
    menu_rect = menu_text.get_rect(center=(width / 2, 100))
    screen.blit(menu_text, menu_rect)

    create_game_button = Button(
        image=None,
        pos=(width / 2, 300),
        text_input=create_game_btn_text,
        font=get_font(75),
        base_color=color_palette["secondary"],
        hovering_color=color_palette["primary"]
    )
    blit_rect_behind_button(screen, create_game_button, 175, color_palette["primary_dark"])
    join_game_button = Button(
        image=None,
        pos=(width / 2, 450),
        text_input=join_game_btn_text,
        font=get_font(75),
        base_color=color_palette["secondary"],
        hovering_color=color_palette["primary"]
    )
    blit_rect_behind_button(screen, join_game_button, 175, color_palette["primary_dark"])
    quit_button = Button(
        image=None,
        pos=(width / 2, 600),
        text_input=quit_btn_text,
        font=get_font(75),
        base_color=color_palette["secondary_dark"],
        hovering_color=color_palette["red"]
    )
    blit_rect_behind_button(screen, quit_button, 175, color_palette["primary_dark"])
    for button in [create_game_button, join_game_button, quit_button]:
        button.change_color(mouse_pos)
        button.update(screen)

    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            pygame.quit()
            sys.exit(0)
        if event.type == pygame.MOUSEBUTTONDOWN:
            if create_game_button.check_for_input(mouse_pos):
                gui_input = "CREATE_GAME"
            if join_game_button.check_for_input(mouse_pos):
                gui_input = "LIST_GAMES"
            if quit_button.check_for_input(mouse_pos):
                gui_input = "LOGOUT"

    clock.tick(60)
    pygame.display.update()


def waiting_for_players(args):
    screen.fill(color_palette["black"])

    mouse_pos = pygame.mouse.get_pos()

    global elapsed_time, waiting_for_players_text, gui_input
    if elapsed_time < int(pygame.time.get_ticks() / 1000):
        elapsed_time = int(pygame.time.get_ticks() / 1000)
        waiting_for_players_text = change_dots(waiting_for_players_text)

    gui_input = ""

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
            sys.exit(0)
        if event.type == pygame.MOUSEBUTTONDOWN:
            if back.check_for_input(mouse_pos):
                gui_input = "LEAVE_GAME"

    clock.tick(60)
    pygame.display.update()


def list_of_games(args):
    screen.fill(color_palette["black"])

    mouse_pos = pygame.mouse.get_pos()

    global gui_input
    gui_input = ""

    game_buttons = []
    page = args[1]
    for i, game in enumerate(args[0]):
        if i < page * 5 or i >= (page + 1) * 5:
            continue

        game_id = game.split(",")[0]
        opponent_name = game.split(",")[1]

        game_button = Button(
            image=None,
            pos=(width / 2, (i % 5) * 100 + 100),
            text_input=game_id + ": vs. " + opponent_name,
            font=get_font(50),
            base_color=color_palette["secondary"],
            hovering_color=color_palette["green"]
        )
        blit_rect_behind_button(screen, game_button, 175, color_palette["primary_dark"])

        game_button.change_color(mouse_pos)
        game_button.update(screen)
        game_buttons.append(game_button)

    back = Button(
        image=None,
        pos=(9 * width / 10, 9 * height / 10),
        text_input=back_btn_text,
        font=get_font(25),
        base_color=color_palette["secondary_dark"],
        hovering_color=color_palette["red"]
    )
    blit_rect_behind_button(screen, back, 175, color_palette["primary_dark"])
    next = Button(
        image=None,
        pos=(width / 2 + 100, 9 * height / 10),
        text_input=">",
        font=get_font(25),
        base_color=color_palette["secondary_dark"],
        hovering_color=color_palette["green"]
    )
    blit_rect_behind_button(screen, next, 175, color_palette["primary_dark"])
    previous = Button(
        image=None,
        pos=(width / 2 - 100, 9 * height / 10),
        text_input="<",
        font=get_font(25),
        base_color=color_palette["secondary_dark"],
        hovering_color=color_palette["green"]
    )
    blit_rect_behind_button(screen, previous, 175, color_palette["primary_dark"])

    btns = [back, next, previous]
    for btn in btns:
        btn.change_color(mouse_pos)
        btn.update(screen)

    page_text = get_font(25).render(str(page), True, color_palette["primary"])
    page_rect = page_text.get_rect(center=(width / 2, 9 * height / 10))
    screen.blit(page_text, page_rect)

    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            pygame.quit()
            sys.exit(0)
        if event.type == pygame.MOUSEBUTTONDOWN:
            if back.check_for_input(mouse_pos):
                gui_input = "BACK_TO_MAIN_MENU"
            if next.check_for_input(mouse_pos):
                gui_input = "NEXT_PAGE"
            if previous.check_for_input(mouse_pos):
                gui_input = "PREVIOUS_PAGE"
            for i, game_button in enumerate(game_buttons):
                if game_button.check_for_input(mouse_pos):
                    gui_input = "JOIN_GAME|" + args[0][i].split(",")[0]

    clock.tick(60)
    pygame.display.update()


def play(args):
    screen.blit(game_bg, (0, 0))

    mouse_pos = pygame.mouse.get_pos()

    global gui_input, player_dice_buttons, opponent_dice_buttons, dice_buttons_initialized, selected
    gui_input = ""

    if len(args) != 9:
        return

    player_nick = args[4]
    opponent_nick = args[5]

    player_dice = [dice[args[0][0]], dice[args[0][1]], dice[args[0][2]], dice[args[0][3]], dice[args[0][4]]]
    opponent_dice = [dice[args[1][0]], dice[args[1][1]], dice[args[1][2]], dice[args[1][3]], dice[args[1][4]]]
    player_dice_buttons, opponent_dice_buttons = init_players_dice_buttons(player_dice, opponent_dice)

    player_score = args[2]
    opponent_score = args[3]

    player_nick_text = get_font(40).render(player_nick, True, color_palette["secondary"])
    player_nick_rect = player_nick_text.get_rect(center=(width / 2, 2 * height / 3 - 50))
    screen.blit(player_nick_text, player_nick_rect)

    opponent_nick_text = get_font(40).render(opponent_nick, True, color_palette["secondary"])
    opponent_nick_rect = opponent_nick_text.get_rect(center=(width / 2, height / 3 + 50))
    screen.blit(opponent_nick_text, opponent_nick_rect)

    player_score_text = get_font(25).render(str(player_score), True, color_palette["secondary"])
    player_score_rect = player_score_text.get_rect(center=(width / 2, 2 * height / 3))
    screen.blit(player_score_text, player_score_rect)

    opponent_score_text = get_font(25).render(str(opponent_score), True, color_palette["secondary"])
    opponent_score_rect = opponent_score_text.get_rect(center=(width / 2, height / 3))
    screen.blit(opponent_score_text, opponent_score_rect)

    if args[8] != "":
        game_over_text = get_font(50).render(args[8], True, color_palette["secondary"])
        game_over_rect = game_over_text.get_rect(center=(width / 2, height / 2))
        screen.blit(game_over_text, game_over_rect)

    reroll = Button(
        image=None,
        pos=(9 * width / 10, 9 * height / 10),
        text_input=reroll_btn_text,
        font=get_font(25),
        base_color=color_palette["secondary"],
        hovering_color=color_palette["primary"]
    )
    blit_rect_behind_button(screen, reroll, 175, color_palette["primary_dark"])
    back = Button(
        image=None,
        pos=(9 * width / 10 - 50, height / 10),
        text_input=leave_game_btn_text,
        font=get_font(25),
        base_color=color_palette["secondary_dark"],
        hovering_color=color_palette["red"]
    )
    blit_rect_behind_button(screen, back, 175, color_palette["primary_dark"])
    if not args[6] and args[7] and args[8] == "":
        accept_end_of_round = Button(
            image=None,
            pos=(width / 2, height / 2),
            text_input=accept_end_of_round_btn_text,
            font=get_font(25),
            base_color=color_palette["secondary_dark"],
            hovering_color=color_palette["green"]
        )
        blit_rect_behind_button(screen, accept_end_of_round, 175, color_palette["primary_dark"])
        accept_end_of_round.change_color(mouse_pos)
        accept_end_of_round.update(screen)

    for i, button in enumerate(player_dice_buttons):
        if selected[i] == 1:
            button.base_color = color_palette["primary"]
            button.hovering_color = color_palette["red"]
        else:
            button.base_color = color_palette["secondary"]
            button.hovering_color = color_palette["green"]

    for button in player_dice_buttons + opponent_dice_buttons + [reroll, back]:
        button.change_color(mouse_pos)
        button.update(screen)

    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            pygame.quit()
            sys.exit(0)
        if event.type == pygame.MOUSEBUTTONDOWN:
            if back.check_for_input(mouse_pos):
                gui_input = "LEAVE_GAME"
            if reroll.check_for_input(mouse_pos):
                if args[6] and args[8] == "":
                    gui_input = "REROLL|" + str(selected[0]) + "," + str(selected[1]) + "," + str(selected[2]) + "," + str(selected[3]) + "," + str(selected[4])
                    selected = [0, 0, 0, 0, 0]
            if not args[6] and args[7] and args[8] == "":
                if accept_end_of_round.check_for_input(mouse_pos):
                    gui_input = "ACCEPT_END_OF_ROUND"
            for i, button in enumerate(player_dice_buttons):
                if args[6] and args[8] == "" and button.check_for_input(mouse_pos):
                    if selected[i] == 0:
                        selected[i] = 1
                    else:
                        selected[i] = 0

    clock.tick(60)
    pygame.display.update()
