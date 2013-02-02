#include <RequestsResponses.h>


const QString Response::OK = "OK";
const QString Response::FAILED = "FAILED";

const QString Request::LOGIN = "LOGIN";
const QString Request::GET_PLAYERS = "GET_PLAYERS";
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
