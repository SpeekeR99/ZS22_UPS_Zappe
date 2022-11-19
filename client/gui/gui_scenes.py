import sys
from gui.gui_utils import *
from gui.button import Button

# Initialize pygame font
pygame.font.init()

# Screen based on the width and height of the screen
screen = pygame.display.set_mode((width, height))
# Clock for the game
clock = pygame.time.Clock()
# Set the window title
pygame.display.set_caption(window_name)
# Create a menu background image with the given width and height
menu_bg = pygame.transform.scale(pygame.image.load(menu_bg_pic_path), (width, height))
# Create a game background image with the given width and height
game_bg = pygame.transform.scale(pygame.image.load(game_bg_pic_path), (width, height))

# Selected dice to reroll
selected = [0, 0, 0, 0, 0]
# Elapsed time for the changing dots function
elapsed_time = 0

# GUI Input for the socket part
gui_input = ""


def main_menu(args):
    """
    Main menu scene of the game
    Scene includes three buttons: Create game, Join game and Quit
    :param args: Arguments for the scene (Not used)
    """

    # Fill the screen with the main menu background image
    screen.blit(menu_bg, (0, 0))

    # Reset the elapsed time and GUI input
    global elapsed_time, gui_input
    elapsed_time = 0
    gui_input = ""

    # Get the mouse position
    mouse_pos = pygame.mouse.get_pos()

    # Create the MAIN MENU text
    menu_text = get_font(100).render(main_menu_text, True, color_palette["primary"])
    menu_rect = menu_text.get_rect(center=(width / 2, 100))
    screen.blit(menu_text, menu_rect)

    # Create the CREATE GAME button
    create_game_button = Button(
        image=None,
        pos=(width / 2, 300),
        text_input=create_game_btn_text,
        font=get_font(75),
        base_color=color_palette["secondary"],
        hovering_color=color_palette["primary"]
    )
    blit_rect_behind_button(screen, create_game_button, 175, color_palette["primary_dark"])

    # Create the JOIN GAME button
    join_game_button = Button(
        image=None,
        pos=(width / 2, 450),
        text_input=join_game_btn_text,
        font=get_font(75),
        base_color=color_palette["secondary"],
        hovering_color=color_palette["primary"]
    )
    blit_rect_behind_button(screen, join_game_button, 175, color_palette["primary_dark"])

    # Create the QUIT button
    quit_button = Button(
        image=None,
        pos=(width / 2, 600),
        text_input=quit_btn_text,
        font=get_font(75),
        base_color=color_palette["secondary_dark"],
        hovering_color=color_palette["red"]
    )
    blit_rect_behind_button(screen, quit_button, 175, color_palette["primary_dark"])

    # Update the buttons and check for hovering
    for button in [create_game_button, join_game_button, quit_button]:
        button.change_color(mouse_pos)
        button.update(screen)

    # Handle events
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            pygame.quit()
            sys.exit(0)
        if event.type == pygame.MOUSEBUTTONDOWN:
            # Check if the CREATE GAME button is pressed
            if create_game_button.check_for_input(mouse_pos):
                gui_input = "CREATE_GAME"
            # Check if the JOIN GAME button is pressed
            if join_game_button.check_for_input(mouse_pos):
                gui_input = "LIST_GAMES"
            # Check if the QUIT button is pressed
            if quit_button.check_for_input(mouse_pos):
                gui_input = "LOGOUT"

    # Update the display
    clock.tick(60)
    pygame.display.update()


def waiting_for_opponent(args):
    """
    Waiting for players scene of the game
    Scene only has a changing dots animation text
    :param args: Arguments for the scene (Not used)
    """

    # Fill the screen with the black color
    screen.fill(color_palette["black"])

    # Get the mouse position
    mouse_pos = pygame.mouse.get_pos()

    # Modify the WAITING FOR OPPONENT text
    global elapsed_time, waiting_for_players_text, gui_input
    if elapsed_time < int(pygame.time.get_ticks() / 1000):
        elapsed_time = int(pygame.time.get_ticks() / 1000)
        waiting_for_players_text = change_dots(waiting_for_players_text)

    # Reset the GUI input
    gui_input = ""

    # Create the WAITING FOR OPPONENT text
    loading_text = get_font(50).render(waiting_for_players_text, True, color_palette["secondary"])
    loading_rect = loading_text.get_rect(center=(width / 2, height / 2))
    screen.blit(loading_text, loading_rect)

    # Create the BACK button
    back = Button(
        image=None,
        pos=(9 * width / 10, 9 * height / 10),
        text_input=back_btn_text,
        font=get_font(25),
        base_color=color_palette["secondary_dark"],
        hovering_color=color_palette["red"]
    )
    blit_rect_behind_button(screen, back, 175, color_palette["primary_dark"])

    # Update the button and check for hovering
    back.change_color(mouse_pos)
    back.update(screen)

    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            pygame.quit()
            sys.exit(0)
        if event.type == pygame.MOUSEBUTTONDOWN:
            # Check if the BACK button is pressed
            if back.check_for_input(mouse_pos):
                gui_input = "LEAVE_GAME"

    # Update the display
    clock.tick(60)
    pygame.display.update()


def list_of_games(args):
    """
    List of games scene of the game
    Scenes includes a list of games and a BACK button
    List of games is a problem, because it could be really long
    Solution: Make the list book like, so you can turn the pages
    Thus two more buttons are needed, one for the previous page and one for the next page
    :param args: Arguments for the scene
                 args[0] - List of games
                 args[1] - Current page
    """

    # Fill the screen with the black color
    screen.fill(color_palette["black"])

    # Get the mouse position
    mouse_pos = pygame.mouse.get_pos()

    # Reset the GUI input
    global gui_input
    gui_input = ""

    # Create the GAME BUTTONS of the current page
    game_buttons = []
    page = args[1]
    for i, game in enumerate(args[0]):
        # Skip the games that are not on the current page
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

        game_buttons.append(game_button)

    # Create the BACK button
    back = Button(
        image=None,
        pos=(9 * width / 10, 9 * height / 10),
        text_input=back_btn_text,
        font=get_font(25),
        base_color=color_palette["secondary_dark"],
        hovering_color=color_palette["red"]
    )
    blit_rect_behind_button(screen, back, 175, color_palette["primary_dark"])

    # Create the NEXT PAGE button
    next = Button(
        image=None,
        pos=(width / 2 + 100, 9 * height / 10),
        text_input=">",
        font=get_font(25),
        base_color=color_palette["secondary_dark"],
        hovering_color=color_palette["green"]
    )
    blit_rect_behind_button(screen, next, 175, color_palette["primary_dark"])

    # Create the PREVIOUS PAGE button
    previous = Button(
        image=None,
        pos=(width / 2 - 100, 9 * height / 10),
        text_input="<",
        font=get_font(25),
        base_color=color_palette["secondary_dark"],
        hovering_color=color_palette["green"]
    )
    blit_rect_behind_button(screen, previous, 175, color_palette["primary_dark"])

    # Update the buttons and check for hovering
    btns = [back, next, previous] + game_buttons
    for btn in btns:
        btn.change_color(mouse_pos)
        btn.update(screen)

    # Render the current page number
    page_text = get_font(25).render(str(page), True, color_palette["primary"])
    page_rect = page_text.get_rect(center=(width / 2, 9 * height / 10))
    screen.blit(page_text, page_rect)

    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            pygame.quit()
            sys.exit(0)
        if event.type == pygame.MOUSEBUTTONDOWN:
            # Check if the BACK button is pressed
            if back.check_for_input(mouse_pos):
                gui_input = "BACK_TO_MAIN_MENU"
            # Check if the NEXT PAGE button is pressed
            if next.check_for_input(mouse_pos):
                gui_input = "NEXT_PAGE"
            # Check if the PREVIOUS PAGE button is pressed
            if previous.check_for_input(mouse_pos):
                gui_input = "PREVIOUS_PAGE"
            # Check if a GAME button is pressed and try to join the game
            for i, game_button in enumerate(game_buttons):
                if game_button.check_for_input(mouse_pos):
                    gui_input = "JOIN_GAME|" + args[0][i].split(",")[0]

    # Update the display
    clock.tick(60)
    pygame.display.update()


def play(args):
    """
    Play scene of the game (the actual game)
    Game consists of a board, information text (nicknames, points), dice of the players
    and a button for REROLL, END TURN and LEAVE GAME
    :param args: Arguments for the scene
                 args[0] - Player dice
                 args[1] - Opponent dice
                 args[2] - Player score
                 args[3] - Opponent score
                 args[4] - Player nickname
                 args[5] - Opponent nickname
                 args[6] - Player turn
                 args[7] - Enemy turn
                 args[8] - Game over text
    """

    # Fill the screen with the game board image
    screen.blit(game_bg, (0, 0))

    # Get the mouse position
    mouse_pos = pygame.mouse.get_pos()

    # Reset the GUI input
    global gui_input, player_dice_buttons, opponent_dice_buttons, dice_buttons_initialized, selected
    gui_input = ""

    # For the first time of the scene, it could get no args at all (safety check)
    if len(args) != 9:
        return

    # Players dice from args (args[0] - player dice, args[1] - opponent dice)
    player_dice = [dice[args[0][0]], dice[args[0][1]], dice[args[0][2]], dice[args[0][3]], dice[args[0][4]]]
    opponent_dice = [dice[args[1][0]], dice[args[1][1]], dice[args[1][2]], dice[args[1][3]], dice[args[1][4]]]

    # Players score from args (args[2] - player score, args[3] - opponent score)
    player_score = args[2]
    opponent_score = args[3]

    # Players nicknames from args (args[4] - player nickname, args[5] - opponent nickname)
    player_nick = args[4]
    opponent_nick = args[5]

    # Player turn from args (args[6] - player turn, args[7] - opponent turn)
    player_turn = args[6]
    opponent_turn = args[7]

    # Game over text from args (args[8])
    game_over = args[8]

    # Render the player dice buttons
    player_dice_buttons, opponent_dice_buttons = init_players_dice_buttons(player_dice, opponent_dice)

    # Render the player nickname
    player_nick_text = get_font(40).render(player_nick, True, color_palette["secondary"])
    player_nick_rect = player_nick_text.get_rect(center=(width / 2, 2 * height / 3 - 50))
    screen.blit(player_nick_text, player_nick_rect)

    # Render the opponent nickname
    opponent_nick_text = get_font(40).render(opponent_nick, True, color_palette["secondary"])
    if opponent_nick.find("Disconnected") != -1:
        opponent_nick_text = get_font(40).render(opponent_nick, True, color_palette["secondary_dark"])
    opponent_nick_rect = opponent_nick_text.get_rect(center=(width / 2, height / 3 + 50))
    screen.blit(opponent_nick_text, opponent_nick_rect)

    # Render the player score
    player_score_text = get_font(25).render(str(player_score), True, color_palette["secondary"])
    player_score_rect = player_score_text.get_rect(center=(width / 2, 2 * height / 3))
    screen.blit(player_score_text, player_score_rect)

    # Render the opponent score
    opponent_score_text = get_font(25).render(str(opponent_score), True, color_palette["secondary"])
    opponent_score_rect = opponent_score_text.get_rect(center=(width / 2, height / 3))
    screen.blit(opponent_score_text, opponent_score_rect)

    # Render the game over text or the ACCEPT END OF ROUND button
    accept_end_of_round = None
    # If the game is over, render the text given to the player
    if game_over != "":
        game_over_text = get_font(50).render(game_over, True, color_palette["secondary"])
        game_over_rect = game_over_text.get_rect(center=(width / 2, height / 2))
        screen.blit(game_over_text, game_over_rect)
    # If the game is not over, render the ACCEPT END OF ROUND button
    else:
        accept_end_of_round = Button(
            image=None,
            pos=(width / 2, height / 2),
            text_input=accept_end_of_round_btn_text,
            font=get_font(25),
            base_color=color_palette["secondary_dark"],
            hovering_color=color_palette["green"]
        )
        blit_rect_behind_button(screen, accept_end_of_round, 175, color_palette["primary_dark"])
        # Update and change the button color if it is hovered over by the mouse
        accept_end_of_round.change_color(mouse_pos)
        accept_end_of_round.update(screen)

    # Render the REROLL button
    reroll = Button(
        image=None,
        pos=(9 * width / 10, 9 * height / 10),
        text_input=reroll_btn_text,
        font=get_font(25),
        base_color=color_palette["secondary"],
        hovering_color=color_palette["primary"]
    )
    blit_rect_behind_button(screen, reroll, 175, color_palette["primary_dark"])

    # Render the LEAVE GAME button
    back = Button(
        image=None,
        pos=(9 * width / 10 - 50, height / 10),
        text_input=leave_game_btn_text,
        font=get_font(25),
        base_color=color_palette["secondary_dark"],
        hovering_color=color_palette["red"]
    )
    blit_rect_behind_button(screen, back, 175, color_palette["primary_dark"])

    # Update and change the button color if it is hovered over by the mouse
    for i, button in enumerate(player_dice_buttons):
        if selected[i] == 1:
            button.base_color = color_palette["primary"]
            button.hovering_color = color_palette["red"]
        else:
            button.base_color = color_palette["secondary"]
            button.hovering_color = color_palette["green"]

    # Update and change the button color if it is hovered over by the mouse
    for button in player_dice_buttons + opponent_dice_buttons + [reroll, back]:
        button.change_color(mouse_pos)
        button.update(screen)

    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            pygame.quit()
            sys.exit(0)
        if event.type == pygame.MOUSEBUTTONDOWN:
            # Check if the LEAVE GAME button is pressed
            if back.check_for_input(mouse_pos):
                gui_input = "LEAVE_GAME"
            # Check if the REROLL button is pressed
            if reroll.check_for_input(mouse_pos):
                # Check if the player can play and if the game is not over
                if player_turn and game_over == "":
                    gui_input = "REROLL|" + str(selected[0]) + "," + str(selected[1]) + "," + str(
                        selected[2]) + "," + str(selected[3]) + "," + str(selected[4])
                    selected = [0, 0, 0, 0, 0]  # Reset the selected dice
            # Check if the ACCEPT END OF ROUND button is pressed
            if accept_end_of_round is not None and accept_end_of_round.check_for_input(mouse_pos):
                # Check if the player and opponent have both played, so it is the end of the round
                if not player_turn and not opponent_turn:
                    gui_input = "ACCEPT_END_OF_ROUND"
            # Check if the player dice buttons are pressed
            for i, button in enumerate(player_dice_buttons):
                # Check if the player can play and if the game is not over
                if player_turn and game_over == "" and button.check_for_input(mouse_pos):
                    if selected[i] == 0:
                        selected[i] = 1
                    else:
                        selected[i] = 0

    # Update the screen
    clock.tick(60)
    pygame.display.update()


def disconnected(args):
    """
    This function is called when the server is unavailable and the client gets disconnected
    :param args: Arguments for the scene
    """

    # Fill the screen with the black color
    screen.fill(color_palette["black"])

    # Get the mouse position
    mouse_pos = pygame.mouse.get_pos()

    # Modify the WAITING FOR OPPONENT text
    global elapsed_time, reconnecting_text, gui_input
    if elapsed_time < int(pygame.time.get_ticks() / 1000):
        elapsed_time = int(pygame.time.get_ticks() / 1000)
        reconnecting_text = change_dots(reconnecting_text)

    # Reset the GUI input
    gui_input = ""

    # Create the MAIN MENU text
    server_down_text = get_font(75).render(server_is_down_text, True, color_palette["red"])
    server_down_rect = server_down_text.get_rect(center=(width / 2, 100))
    screen.blit(server_down_text, server_down_rect)

    # Create the RECONNECTING text
    loading_text = get_font(50).render(reconnecting_text, True, color_palette["secondary"])
    loading_rect = loading_text.get_rect(center=(width / 2, height / 2))
    screen.blit(loading_text, loading_rect)

    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            pygame.quit()
            sys.exit(0)

    # Update the display
    clock.tick(60)
    pygame.display.update()
