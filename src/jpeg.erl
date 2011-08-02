-module(jpeg).

-on_load(start/0).
-export([start/0]).
-export([decode/1]).

start() ->
  Path = case code:lib_dir(jpeg,priv) of
    {error,_ } -> "priv";
    Else -> Else
  end,
  erlang:load_nif(Path++ "/jpeg", 0).


decode(_Jpeg) ->
  erlang:error(nif_not_loaded).