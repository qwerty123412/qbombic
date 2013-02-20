#include <RequestsResponses.h>


const QString Response::OK = "OK";
const QString Response::FAILED = "FAILED";

const QString Request::LOGIN = "LOGIN";
const QString Request::GET_PLAYERS = "GET_PLAYERS";
const QString Request::GET_GAMES = "GET_GAMES";
const QString Request::CREATE_GAME = "CREATE_GAME";
const QString Request::JOIN_GAME = "JOIN_GAME";
const QString Request::START_GAME = "START_GAME";

const QString Notifications::NEW_PLAYER = "NEW_PLAYER";
const QString Notifications::QUIT_PLAYER = "QUIT_PLAYER";
const QString Notifications::GAME_CLOSED = "GAME_CLOSED";
const QString Notifications::GAME_LIST = "GAME_LIST";
const QString Notifications::MESSAGE = "MESSAGE";
const QString Notifications::QUIT_GAME = "QUIT_GAME";
const QString Notifications::GAME_STARTED = "GAME_STARTED";
const QString Notifications::GAME_STOPPED = "GAME_STOPPED";
const QString Notifications::GAME_STATE = "GAME_STATE";

const QString GameEvents::MOVE_DOWN = "M_DOWN";
const QString GameEvents::MOVE_LEFT = "M_LEFT";
const QString GameEvents::MOVE_RIGHT = "M_RIGHT";
const QString GameEvents::MOVE_UP = "M_UP";
const QString GameEvents::BOMB_LAID = "BOMB_LAID";

const QString GameDataObjects::BOMB = "BOMB";
const QString GameDataObjects::FIRE = "FIRE";
const QString GameDataObjects::PLAYER = "PLAYER";
const QString GameDataObjects::WALL = "WALL";
const QString GameDataObjects::POWERUP = "POWERUP";
const QString GameDataObjects::UNDESTROYABLE = "UNDESTROYABLE";
const QString GameDataObjects::P_NAME = "NAME";
const QString GameDataObjects::P_COORDINATIONS = "COORDINATIONS";
const QString GameDataObjects::P_BOMBS = "BOMBS";
const QString GameDataObjects::P_DEATHS = "DEATHS";
const QString GameDataObjects::P_KILLS = "KILLS";
