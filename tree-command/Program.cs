using System;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Collections.Generic;

namespace tree_command_csharp
{
    class Program
    {
        // Constants for drawing lines and spaces
        private const string _cross = "   ├─";
        private const string _corner = "   └─";
        private const string _vertical = "   │ ";
        private const string _space = "     ";

        public static void Main(params string [] args)
        {
            // currently only works if you remove the final slash in the path
            // program breaks if an UnauthorizedUseException is hit
            if (args.Length != 1)
            {
                Console.WriteLine($"Usage: {System.AppDomain.CurrentDomain.FriendlyName} path");
                Environment.Exit(1);
            }

            Node root = new Node(args[0], null, true);
            PrintNode(root, "");
        }

        internal static void PrintNode(Node node, string indent)
        {

            Console.WriteLine(GetNameWithoutPath(node.Name));
            // print files after the directory name
            if (node.Files != null)
            {
                foreach (var f in node.Files)
                {
                    if (node.Children.Count > 0)
                        Console.WriteLine($"{indent}{_vertical} {GetNameWithoutPath(f)}");
                    else
                        Console.WriteLine($"{indent}{_space} {GetNameWithoutPath(f)}");
                }
            }

            // loop through children recursively, passing in indent and isLastDir
            int numberOfChildren = (node.Children != null) ? node.Children.Count : 0;

        ReturnFromException:

            try
            {
                for (int c = 0; c < numberOfChildren; c++)
                {
                    if (node != null && node.Children[c] != null)
                    {
                        if (c == numberOfChildren - 1)
                            PrintChildNode(node.Children[c], node, indent, true);
                        else
                            PrintChildNode(node.Children[c], node, indent, false);
                    }
                }
            }
            catch (UnauthorizedAccessException ex)
            {
                Trace.WriteLine(ex.Message);
                goto ReturnFromException;
            }
        }

        static void PrintChildNode(Node node, Node parent, string indent, bool isLast)
        {
            // Print the provided pipes/spaces indent
            Console.Write(indent);
            // Depending if this node is a last child, print the
            // corner or cross, and calculate the indent that will
            // be passed to its children
            if (isLast)
            {
                Console.Write(_corner);
                indent += _space;
            }
            else
            {
                Console.Write(_cross);
                indent += _vertical;
            }

            PrintNode(node, indent);
        }

        static string GetNameWithoutPath(string path) => path.Substring(path.LastIndexOf('\\') + 1);

        public class Node
        {
            public string Name;
            public List<Node> Children;
            public string[] Files;

            public Node(string name, Node parent, bool lastDir)
            {
                Name = name;

                try
                {
                    var dirs = Directory.GetDirectories(Name).OrderBy(n => n).ToArray();
                    Children = new List<Node>(dirs.Length);

                    for (int i = 0; i < dirs.Length; i++)
                    {
                        if (i != dirs.Length - 1)
                            Children.Add(new Node(dirs[i], this, false));
                        else
                            Children.Add(new Node(dirs[i], this, true));
                    }
                }
                catch (UnauthorizedAccessException ex)
                {
                    Trace.WriteLine(ex.Message);
                }

                try
                {
                    Files = Directory.GetFiles(Name);
                }
                catch (UnauthorizedAccessException ex)
                {
                    Trace.WriteLine(ex.Message);
                }
            }
        }
    }
}
