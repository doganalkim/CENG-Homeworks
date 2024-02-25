party(peoples_parks_party, p).
party(peoples_wealth_party, w).
party(dentists_wealth_party, d).

candidate(leslie_knope, peoples_parks_party, pawnee, 1).
candidate(ben_wyatt, peoples_parks_party, pawnee, 2).
candidate(april_ludgate, peoples_parks_party, pawnee, 3).
candidate(tom_haverford, peoples_parks_party, pawnee, 4).
candidate(jerry_gerwich, peoples_parks_party, pawnee, 5).
candidate(jeremy_jamm, dentists_wealth_party, pawnee, 1).
candidate(joe_fantringham, peoples_wealth_party, pawnee, 1).
candidate(bill_dexhart, peoples_wealth_party, pawnee, 2).
candidate(craig_middlebrooks, peoples_parks_party, eagleton, 1).
candidate(douglass_howser, peoples_parks_party, eagleton, 2).
candidate(ingrid_de_forest, peoples_wealth_party, eagleton, 1).
candidate(lindsay_carlisle_shay, peoples_wealth_party, eagleton, 2).
candidate(george_gernway, peoples_wealth_party, eagleton, 3).
candidate(joan_callamezzo, peoples_wealth_party, eagleton, 4).

elected(pawnee, peoples_parks_party, 4).
elected(eagleton, peoples_wealth_party, 4).
elected(eagleton, peoples_parks_party, 2).

to_elect(10).

is_vote_wasted(CTY,PARTY) :- not(elected(CTY,PARTY,Z)).

is_candidate_elected(NAME,PARTY) :- candidate(NAME,PARTY,CTY,Z), elected(CTY,PARTY,T), Z =< T.

candidate_count_from_city([],City,0).
candidate_count_from_city([H|REM],City,N) :-  not(candidate(H,Y,City,Z)), candidate_count_from_city(REM,City,N).
candidate_count_from_city([H|REM],City,N) :-  candidate(H,Y,City,Z), candidate_count_from_city(REM,City,M), N is M+1.

 
all_parties(Lst) :- findall(X,party(X,Y),L), L = Lst.

all_candidates_from_party(PARTY,LST) :- findall(NAME,candidate(NAME,PARTY,CTY,N),L), L = LST.

all_elected_from_party(PARTY,LST) :- findall(NAME,is_candidate_elected(NAME,PARTY),L), L=LST.

election_rate(PARTY,PR) :- all_candidates_from_party(PARTY,LSTCAND), all_elected_from_party(PARTY,LSTELECT), length(LSTCAND,NCAND),length(LSTELECT,NELECT), PR is NELECT/NCAND.

council_percentage(PARTY,PR): findall(NAME,candidate(NAME,P,CTY,N),L), all_elected_from_party(PART,LSTELECT), length(LSTELECT,NELECT),length(L,NCAND), PR is NELECT/NCAND*100.

sumlst([],0).
sumlst([H|REM],RES) :- sumlst(REM,RR), RES is RR + H.

council_percentage(PARTY,PR) :- findall(NUM,elected(CTY,PARTY,NUM),LSTELECT), findall(NUMB,elected(CT,GENPAR,NUMB),LSTALL),  sumlst(LSTELECT,NUME), sumlst(LSTALL,NUMALL), PR is NUME/NUMALL.

party_finder(S,PARTY)  :- party(PARTY,S).
candidate_finder(PARTY,NAME) :- candidate(NAME,PARTY,CTY,Numb).

append(X,[],[X]).
append(X,LST,[X|LST]). 

mmember(X, [X|R]).
mmember(X,[F|R]) :- mmember(X,R).

all_p([],[]).
all_p([H|REM],RES) :- party_finder(H,PARTY), all_p(REM,RR),!, append(PARTY,RR,RES).

all_parties(A,RES) :- atom_chars(A,RR), all_p(RR,RES).

members([],[]).
members([HP|REMP],RES) :- candidate(X,HP,Y,Z), members(REMP,RR), not(mmember(X,RR)), once(append(X,RR,RES)).


alternative_debate_setups(S,RES) :-  all_parties(S,RR), members(RR,RES).