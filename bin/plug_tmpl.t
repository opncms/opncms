<% c++ #include "${plug}.h" %>
<% xhtml %>
<% skin %>
<% view ${plug} uses content::${plug} extends base %>

<% template css_head() %>
<% include base::css_head() %>
<link rel="stylesheet" href="<%= media %>/css/${plug}.css" type="text/css">
<% end template %>

<% template js_head() %>
<% include base::js_head() %>
<script src="<%= media %>/js/${plug}.js" type="text/javascript"></script>
<% end template %>

<% template js_foot() %>
<% include base::js_foot() %>
<% end template %>

<% template title() %><%= name %><% end template %>

<% template plugin_content() %>
<form action="" method="post">
<% form as_table ${plug}_form %>
</form>
<% end template %>

<% template render() %>
        <% include base::header() %><% include base::body() %>
<% end template %>

<% end view %>

<% view ${plug}_html_menu uses content::${plug} %>
        <% template render() %>
                <%= name %>
        <% end %>
<% end view %>

<% view ${plug}_html_left uses content::${plug} %>
        <% template render() %>
        <% end %>
<% end view %>

<% view ${plug}_html_right uses content::${plug} %>
        <% template render() %>
        <% end %>
<% end view %>

<% view ${plug}_html_top uses content::${plug} %>
        <% template render() %>
        <% end %>
<% end view %>

<% view ${plug}_html_bottom uses content::${plug} %>
        <% template render() %>
        <% end %>
<% end view %>

<% view ${plug}_html_content uses content::${plug} %>
        <% template render() %>
        <% end %>
<% end view %>

<% end skin %>
