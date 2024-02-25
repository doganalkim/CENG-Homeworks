import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Scanner;

import java.io.File;

public class CengTreeParser
{
    public static ArrayList<CengBook> parseBooksFromFile(String filename)
    {
        ArrayList<CengBook> bookList = new ArrayList<CengBook>();

        // You need to parse the input file in order to use GUI tables.
        // TODO: Parse the input file, and convert them into CengBooks

        try{
            File file = new File(filename);
            Scanner scanner = new Scanner(file);
            while(scanner.hasNextLine()){
                String line = scanner.nextLine();
                System.out.println(line);
                String[] parts = line.split("\\|");
                Integer id = Integer.valueOf(parts[0]);
                String bookTitle = parts[1];
                String author = parts[2];
                String genre = parts[3];
                CengBook cb = new CengBook(id,bookTitle,author,genre);
                bookList.add(cb);
                //System.out.println("Azd");
            }
        }
        catch(FileNotFoundException e){
            e.printStackTrace();
        }


        return bookList;
    }

    public static void startParsingCommandLine() throws IOException
    {
        // TODO: Start listening and parsing command line -System.in-.
        // There are 4 commands:
        // 1) quit : End the app, gracefully. Print nothing, call nothing, just break off your command line loop.
        // 2) add : Parse and create the book, and call CengBookRunner.addBook(newlyCreatedBook).
        // 3) search : Parse the bookID, and call CengBookRunner.searchBook(bookID).
        // 4) print : Print the whole tree, call CengBookRunner.printTree().

        // Commands (quit, add, search, print) are case-insensitive.
        Scanner scanner = new Scanner(System.in);

        while ( true ){
            String input = scanner.nextLine();
            String quit = "quit";
            String add = "add";
            String search = "search";
            String print = "print";
            //System.out.println(input);
            if(input.startsWith(quit)){
                scanner.close();
                break;
            }
            else if(input.startsWith(add)){
                // TO DO
                //System.out.println("add case");
                String[] book = input.split("\\|");
                Integer bookid = Integer.parseInt(book[1]);
                String booktitle = book[2];
                String author = book[3];
                String genre = book[4];
                //System.out.println("the bookid:" + bookid);
                //System.out.println(booktitle);
                //System.out.println(author);
                //System.out.println(genre);
                CengBook cb = new CengBook(bookid,booktitle,author,genre);
                CengBookRunner.addBook(cb);

            }
            else if(input.startsWith(print)){
                // TO DO
                //System.out.println("print case");
                CengBookRunner.printTree();

            }
            else if(input.startsWith(search)){
                // TO DO
                //System.out.println("search case");
                String[] book = input.split("\\|");
                Integer bookid = Integer.parseInt(book[1]);
                CengBookRunner.searchBook(bookid);
            }
            else{
                break;
            }
        }

    }
}
