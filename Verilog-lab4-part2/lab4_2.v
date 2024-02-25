/*
`timescale 1ns / 1ps
module SelectionOfAvatar(
	input [1:0] mode,
	input [5:0] userID,
	input [1:0] candidate, // 00:Air 01:Fire, 10:Earth, 11: Water
	input CLK,
	output reg [1:0] ballotBoxId,
	output reg [5:0] numberOfRegisteredVoters,
	output reg [5:0] numberOfVotesWinner, // number of votes of winner
	output reg [1:0] WinnerId,
	output reg AlreadyRegistered,
	output reg AlreadyVoted,
	output reg NotRegistered,
	output reg VotingHasNotStarted,
	output reg RegistrationHasEnded
	);

	reg [15:0]  c0;
	reg [15:0]  c1; 
	reg [15:0]  c2;
	reg [15:0]  c3;
	reg [63:0]  v;
	reg [5:0]  candidate0; 
	reg [5:0]  candidate1;
	reg [5:0]  candidate2;
	reg [5:0]  candidate3;
 	integer counter;
	reg [3:0] userindex;
	reg [5:0] max;
	reg  [3:0][5:0] loop;
	integer i;
	initial begin
		// ...
		c0 = 0;
		c1 = 0;
		c2 = 0;
		c3 = 0;
		v = 0;	
		counter = 0;
		candidate0 = 0;
		candidate1 = 0;
		candidate2 = 0;
		candidate3 = 0;
		max = 0;
		numberOfRegisteredVoters = 0;
		numberOfVotesWinner = 0; 
		WinnerId=0;
	end

	always @(posedge CLK)
	begin
		// ...
		AlreadyRegistered= 0;
		AlreadyVoted = 0;
		NotRegistered = 0;
		VotingHasNotStarted = 0;
		RegistrationHasEnded = 0;
		userindex = (userID << 2) >> 2;
		counter = counter + 1;
		ballotBoxId = ( userID >> 4) ;
		if(counter <= 100)begin	
			if(mode == 1)begin
				VotingHasNotStarted = 1;
			end
			else begin
				if(ballotBoxId==0) begin
					for(i=0;i<16;i=i+1)begin
						if(i == userindex) begin
							if(c0[i] == 1) begin
								AlreadyRegistered = 1;
							end
							else begin
								c0[i]=1;
								numberOfRegisteredVoters = numberOfRegisteredVoters+1;
							end
						end
					end
						
				end
				else if(ballotBoxId== 1) begin
					for(i=0;i<16;i=i+1)begin
						if(i == userindex) begin
							if(c1[i] == 1) begin
								AlreadyRegistered = 1;
							end
							else begin
								c1[i]=1;
								numberOfRegisteredVoters = numberOfRegisteredVoters+1;
							end
						end
					end
					
				end
				else if(ballotBoxId == 2) begin
					for(i=0;i<16;i=i+1)begin
						if(i == userindex) begin
							if(c2[i] == 1) begin
								AlreadyRegistered = 1;
							end
							else begin
								c2[i]=1;
								numberOfRegisteredVoters = numberOfRegisteredVoters+1;
							end
						end
					end
					
				end
				else begin
					for(i=0;i<16;i=i+1)begin
						if(i == userindex) begin
							if(c3[i] == 1) begin
								AlreadyRegistered = 1;
							end
							else begin
								c3[i]=1;
								numberOfRegisteredVoters = numberOfRegisteredVoters+1;
							end
						end
					end
					
				end

			end
			
		end
		else if(counter <= 200)begin
			if(mode == 0) begin
				RegistrationHasEnded = 1;
			end
			else begin
				if(ballotBoxId== 0) begin
					for(i=0;i<16;i++) begin
						if(i == userindex) begin
							if(c0[i]==0) begin
								NotRegistered = 1;
							end
							else if(  v[userID] == 1 ) begin
								AlreadyVoted = 1;
							end
							else begin
								v[userID] = 1;
								if(candidate==0) begin
									candidate0 = candidate0 + 1;	
								end
								else if(candidate == 1) begin
									candidate1 = candidate1 + 1;
								end
								else if(candidate == 2) begin
									candidate2 = candidate2 + 1;
								end
								else begin
									candidate3 = candidate3 + 1;
								end
							end
						end
					end
				end
				else if(ballotBoxId == 1) begin
					for(i=0;i<16;i++) begin
						if(i == userindex) begin
							if(c1[i]==0) begin
								NotRegistered = 1;
							end
							else if(  v[userID] == 1 ) begin
								AlreadyVoted = 1;
							end
							else begin
								v[userID] = 1;
								if(candidate==0) begin
									candidate0 = candidate0 + 1;	
								end
								else if(candidate == 1) begin
									candidate1 = candidate1 + 1;
								end
								else if(candidate == 2) begin
									candidate2 = candidate2 + 1;
								end
								else begin
									candidate3 = candidate3 + 1;
								end
							end
						end
					end					
				end
				else if(ballotBoxId == 2) begin
					for(i=0;i<16;i++) begin
						if(i == userindex) begin
							if(c2[i]==0) begin
								NotRegistered = 1;
							end
							else if(  v[userID] == 1 ) begin
								AlreadyVoted = 1;
							end
							else begin
								v[userID] = 1;
								if(candidate==0) begin
									candidate0 = candidate0 + 1;	
								end
								else if(candidate == 1) begin
									candidate1 = candidate1 + 1;
								end
								else if(candidate == 2) begin
									candidate2 = candidate2 + 1;
								end
								else begin
									candidate3 = candidate3 + 1;
								end
							end
						end
					end
				end
				else begin
					for(i=0;i<16;i++) begin
						if(i == userindex) begin
							if(c3[i]==0) begin
								NotRegistered = 1;
							end
							else if(  v[userID] == 1 ) begin
								AlreadyVoted = 1;
							end
							else begin
								v[userID] = 1;
								if(candidate==0) begin
									candidate0 = candidate0 + 1;	
								end
								else if(candidate == 1) begin
									candidate1 = candidate1 + 1;
								end
								else if(candidate == 2) begin
									candidate2 = candidate2 + 1;
								end
								else begin
									candidate3 = candidate3 + 1;
								end
							end
						end
					end
				end
			end
		end
		else if(counter > 200)begin
			loop[0]=candidate0;
			loop[1]=candidate1;
			loop[2]=candidate2;
			loop[3]=candidate3;
			max = candidate0;
			WinnerId = 0;
			for(i=0;i<4;i=i+1) begin
				if(loop[i]>max)begin
					max = loop[i];
					WinnerId = i;
				end
			end
			numberOfVotesWinner = max;
		end
	end
endmodule
*/

 